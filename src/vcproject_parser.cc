#include "precompiled.h"
#include "vcproject_parser.h"
#include "utility.h"
#include "string_tokenizer.h"

#ifdef _MSC_VER
//#pragma warning(push)
//#pragma warning(disable: 4706)
#endif

using std::back_inserter;
using std::copy;
using std::copy_if;
using std::function;
using std::ifstream;
using std::make_pair;
using std::move;
using std::remove_if;
using std::string;
using std::unordered_map;
using std::vector;

namespace {
typedef rapidxml::xml_document<>           XMLDocument;
typedef rapidxml::xml_node<>               XMLNode;
typedef rapidxml::xml_attribute<>          XMLAttribute;
typedef std::unordered_map<string, string> MacroMap;
}


// @@kProjectMacros is used to initialize MacroMap's keys.
// Documentation from
// http://msdn.microsoft.com/en-us/library/c02as0cs(VS.90).aspx
static const char* const kProjectMacros2008[] = {
  "$(ConfigurationName)", //The name of the current project configuration (for example, "Debug").
  "$(DevEnvDir)",
  "$(FrameworkDir)",
  "$(FrameworkSDKDir)",
  "$(FrameworkVersion)",
  "$(FxCopDir)"
  "$(Inherit)",
  "$(InputDir)", //The directory of the input file (defined as drive + path); includes the trailing backslash '\'. If the project is the input, then this macro is equivalent to $(ProjectDir).
  "$(InputExt)", //The file extension of the input file. It includes the '.' before the file extension. If the project is the input, then this macro is equivalent to $(ProjectExt).
  "$(InputFileName)", //The file name of the input file (defined as base name + file extension). If the project is the input, then this macro is equivalent to $(ProjectFileName).
  "$(InputName)", //The base name of the input file. If the project is the input, then this macro is equivalent to $(ProjectName).
  "$(InputPath)", //The absolute path name of the input file (defined as drive + path + base name + file extension). If the project is the input, then this macro is equivalent to $(ProjectPath).
  "$(IntDir)",
  "$(NoInherit)",
  "$(OutDir)",
  "$(ParentName)",
  "$(PlatformName)", //The name of current project platform (for example, "Win32").
  "$(Platform)", //The name of current project platform (for example, "Win32"). (vs 2010)
  "$(ProjectDir)", //The directory of the input file (defined as drive + path); includes the trailing backslash '\'. If the project is the input, then this macro is equivalent to $(ProjectDir).
  "$(ProjectExt)", //The file extension of the project. It includes the '.' before the file extension.
  "$(ProjectFileName)", //The file name of the project (defined as base name + file extension).
  "$(ProjectName)", //The base name of the project.
  "$(ProjectPath)", //The absolute path name of the project (defined as drive + path + base name + file extension).
  "$(References)",
  "$(RemoteMachine)",
  "$(RootNameSpace)",
  "$(SafeInputName)",
  "$(SafeParentName)",
  "$(SafeRootNamespace)",
  "$(SolutionDir)",
  "$(SolutionExt)",
  "$(SolutionFileName)",
  "$(SolutionName)",
  "$(SolutionPath)",
  "$(StopEvaluating)",
  "$(TargetDir)", //The directory of the primary output file for the build (defined as drive + path); includes the trailing backslash '\'.
  "$(TargetExt)", //The file extension of the primary output file for the build. It includes the '.' before the file extension.
  "$(TargetFileName)", //The file name of the primary output file for the build (defined as base name + file extension).
  "$(TargetName)", //The base name of the primary output file for the build.
  "$(TargetPath)", //The absolute path name of the primary output file for the build (defined as drive + path + base name + file extension).
  "$(VCInstallDir)",
  "$(VSInstallDir)",
  "$(WebDeployPath)",
  "$(WebDeployRoot)"
};

void FilterByNodeName(const string& name, XMLNode* node, vector<XMLNode*>& vec){
    if(!node) return;
    if(name.compare(node->name()) == 0) {
      vec.push_back(node);
    }
    FilterByNodeName(name, node->next_sibling(),vec);
    FilterByNodeName(name, node->first_node(),vec);
}

void FilterByAttribute(const string& name, const string& value, XMLNode* node, vector<XMLNode*>& vec){
    if(!node) return;
     if(XMLAttribute* attr = node->first_attribute(name.c_str())) {
      if(value.compare(attr->value()) == 0)
        vec.push_back(node);
    }
    FilterByAttribute(name, value, node->next_sibling(),vec);
    FilterByAttribute(name, value, node->first_node(),vec);
}

// Traverse all 'Filter' and 'File' nodes and
// return a flat structure containing only "File" nodes
static void CollectFileNodes(const XMLNode* node, std::vector<const XMLNode*>* out) {
  if(!node) return;
  // id File nodes by the existance of a 'RelativePath' attribute
  // id Filter nodes by the existance of a 'Name' attribute
  if(node->first_attribute("Name")) {
    CollectFileNodes(node->first_node(), out);
    CollectFileNodes(node->next_sibling(), out);
  } else if(node->first_attribute("RelativePath")) {
    out->push_back(node);
    CollectFileNodes(node->first_node(), out);
    CollectFileNodes(node->next_sibling(), out);
  }
}

// In place-expansion of macros(see kProjectMacros2008) found in input
static void ExpandMacros(string* input, const MacroMap& macros) {
  for(size_t i = 0; i < ARRAY_COUNT(kProjectMacros2008); ++i) {
    size_t pos = 0;
    // macros should've been initialized with kProjectMacros
    // so we dont need to check the results of find.
    auto iter = macros.find(kProjectMacros2008[i]);
    while((pos = input->find(kProjectMacros2008[i], pos)) != string::npos) {
      input->replace(pos, strlen(kProjectMacros2008[i]), iter->second);
    }
  }
}

void CollectToolProperties(XMLNode* tool,
                           unordered_map<string, unordered_map<string, string> >& properties, const MacroMap& macros) {

  while(tool) {
    XMLAttribute* attr   = tool->first_attribute("Name");
    const char* toolname = attr->value();
    attr = attr->next_attribute();
    while(attr) {
      string value(attr->value());
      ExpandMacros(&value, macros);
      properties[toolname].insert(make_pair(attr->name(), value));
      attr = attr->next_attribute();
    }
    tool = tool->next_sibling();
  }
}

// Returns a map of all tool properties identified
// by toolname
bool GetToolPropertiesForTool(const string& toolname, XMLNode* tool, unordered_map<string, string>* props) {
  while(tool) {
    XMLAttribute* attr = tool->first_attribute("Name");
    if(attr  && (toolname.compare(attr->value()) == 0)) {
      attr = attr->next_attribute();
      while(attr) {
        props->insert(make_pair(attr->name(), attr->value()));
        attr = attr->next_attribute();
      }
      break;
    }
    tool = tool->next_sibling();
  }
  return tool != 0;
}

// Returns a map of all tool properties identified by toolname
// for the specified configuration 'name'
bool GetToolPropertiesForConfiguration(const string& name, const string& toolname,
                                       XMLNode* configuration, unordered_map<string, string>* props) {

  while(configuration) {
    if(XMLAttribute* attr = configuration->first_attribute("Name")) {
      if(name.compare(attr->value()) == 0)
        break;
    }
    configuration = configuration->next_sibling();
  }

  if(!configuration)
    return false;

  return GetToolPropertiesForTool(toolname, configuration->first_node("Tool"), props);
}

VCProjectParser::VCProjectParser()
  : root(0) {
  for(size_t i = 0; i < ARRAY_COUNT(kProjectMacros2008); ++i) {
    macros.insert(make_pair(kProjectMacros2008[i], ""));
  }
}

bool VCProjectParser::ProjectProperties(unordered_map<string, string>* props) {

  static const char* kGlobalProperties[] = {
    "Name",
    "ProjectType",
    "Version",
    "Keyword",
    "RootNamespace",
    "ProjectGUID"
  };

  for(size_t i = 0; i < ARRAY_COUNT(kGlobalProperties); ++i) {
    XMLAttribute* attr = root->first_attribute(kGlobalProperties[i]);
    props->insert(make_pair(kGlobalProperties[i], attr ? attr->value() : ""));
  }

  size_t sep = origin.find_last_of("\\/");

  if(sep != string::npos) {
    macros["$(ProjectDir)"].assign(origin.substr(0, sep));

    // we don't know SolutionDir yet
    macros["$(SolutionDir)"].assign(origin.substr(0, sep));
  } else {
    macros["$(SolutionDir)"].assign(".");
  }

  macros["$(InputName)"]   = (*props)["Name"];
  macros["$(ProjectName)"] = (*props)["Name"];
  macros["$(TargetName)"]  = (*props)["Name"];
  return true;
}

bool VCProjectParser::CompilerProperties(const string& config,
                                      unordered_map<string, string>* props) {
  if(!root->first_node("Configurations"))
    return false;

  XMLNode* configurations = root->first_node("Configurations");
  if(!configurations)
    return false;
  return GetToolPropertiesForConfiguration(config, "VCCLcompilerTool", configurations->first_node("Configuration"), props);
}

bool VCProjectParser::LibrarianProperties(const string& config,
                                       unordered_map<string, string>* props) {
  if(!root->first_node("Configurations"))
    return false;

  XMLNode* configurations = root->first_node("Configurations");
  if(!configurations)
    return false;
  return GetToolPropertiesForConfiguration(config, "VCLibrarianTool", configurations->first_node("Configuration"), props);
}

bool VCProjectParser::LinkerProperties(const string& config,
                                    unordered_map<string, string>* props) {
  if(!root->first_node("Configurations"))
    return false;

  XMLNode* configurations = root->first_node("Configurations");
  if(!configurations)
    return false;
  return GetToolPropertiesForConfiguration(config, "VCLinkerTool", configurations->first_node("Configuration"), props);
}

bool VCProjectParser::Configurations(vector<vs::Configuration>* configurations) {
  if(!root->first_node("Configurations"))
    return false;

  XMLNode* node = root->first_node("Configurations")->first_node();

  while(node) {
    string output_dir("");
    string intermediate_dir("");
    string property_sheets("");

    vs::Configuration config;

    if(XMLAttribute* attr = node->first_attribute("Name")) {
      config.Name.assign(attr->value());
      config.ConfigurationName.assign(config.Name.substr(0, config.Name.find_first_of('|')));
      config.Platform.assign(config.Name.substr(config.Name.find_first_of('|')+1));

      macros["$(ConfigurationName)"] = config.ConfigurationName;
      macros["$(PlatformName)"]      = config.Platform;
    }

    if(XMLAttribute* attr = node->first_attribute("OutputDirectory")) {
      output_dir.assign(attr->value());
      ExpandMacros(&output_dir, macros);
      macros["$(OutDir)"] = output_dir;
    }


    if(XMLAttribute* attr = node->first_attribute("IntermediateDirectory")) {
      intermediate_dir.assign(attr->value());
      ExpandMacros(&intermediate_dir, macros);
      macros["$(IntDir)"] = intermediate_dir;
    }


    if(XMLAttribute* attr = node->first_attribute("InheritedPropertySheets")) {
      property_sheets.assign(attr->value());;
    }


    if(XMLAttribute* attr = node->first_attribute("ConfigurationType")) {
      config.ConfigurationType = (vs::Enum::Type)strtol(attr->value(), 0, 10);
      
    }


    // Collect tool properties
    CollectToolProperties(node->first_node("Tool"), config.ToolProperties, macros);

    if(XMLAttribute* attr = node->first_attribute("InheritedPropertySheets")) {
      char* ptr = strtok(attr->value(), ";");

      while(ptr != NULL) {
        std::string path(ptr);
        ExpandMacros(&path, macros);


        // the property sheet path is relative to the path of project file
        size_t sep = origin.find_last_of("\\/");
        if(sep != string::npos) {
          std::string vsprops_path(origin.substr(0, sep+1));
          vsprops_path.append(path);
          path.assign(AbsoluteFilePath(vsprops_path));
        } else {
          path.assign(AbsoluteFilePath(path));
        }

        config.PropertySheets.push_back(path);
        // apply property sheets for now properties in property
        // sheets override the existing tool properties.
        string buffer("");
        if(FileToString(path, &buffer)) {
          XMLDocument vsprops;
          vsprops.parse<0>(&buffer[0]);
          XMLNode* props = vsprops.first_node("VisualStudioPropertySheet");
          if(XMLAttribute* props_attr  = props->first_attribute("OutputDirectory")) {
            output_dir.assign(props_attr->value());
          }
          if(XMLAttribute* props_attr  = props->first_attribute("IntermediateDirectory")) {
            intermediate_dir.assign(props_attr->value());
          }





          CollectToolProperties(props->first_node("Tool"), config.ToolProperties, macros);
        }
        ptr = strtok(NULL, ";");
      }
    }

    ExpandMacros(&output_dir, macros);
    ExpandMacros(&intermediate_dir, macros);
    ExpandMacros(&property_sheets, macros);

    config.IntermediateDirectory.assign(intermediate_dir);
    config.OutputDirectory.assign(output_dir);

    configurations->push_back(move(config));
    node = node->next_sibling();
  }


  return true;
}

bool VCProjectParser::Filters(vector<vs::Filter>*) {
  return false;
}
#if 0
bool VCProjectParser::Files(vector<vs::Configuration>* configurations, vector<vs::File>* files) {
  if(!root->first_node("Files")) return false;

  vector<const XMLNode*> file_nodes;
  CollectFileNodes(root->first_node("Files")->first_node(), &file_nodes);

  if(file_nodes.empty()) return false;

  // alternative name for easier access to operator[]
  vector<vs::Configuration>& configs = *configurations;


  // reserve each configuration's file list
  files->reserve(file_nodes.size());
  for(size_t i = 0, end = configs.size(); i < end; ++i) {
    configs[i].Files.reserve(file_nodes.size());
  }

  foreach(const XMLNode* node, file_nodes) {

    const XMLAttribute* attr = node->first_attribute("RelativePath");
    if(!attr) continue;

    vs::File file;
    file.Precompiled = false;
    file.CompileAsC = false;
    file.ForcedInclude = false;
    
    // build a list of configurations that will
    // reference the VCFile
    vector<vs::Configuration*> targets;
    for(size_t i = 0, end = configs.size(); i < end; ++i) {
      targets.push_back(&configs[i]);
    }

    file.RelativePath.assign(attr->value());
    file.AbsolutePath.assign(AbsoluteFilePath(file.RelativePath));

    // check per-file FileConfiguration
    const XMLNode* file_configuration = node->first_node("FileConfiguration");
    while(file_configuration) {

      // Remove the configuration that matches 'config_name'
      // from the list of targets
      // since the vcfile is ExcludedFromBuild
      const XMLAttribute* config_name = file_configuration->first_attribute("Name");
      if(file_configuration->first_attribute("ExcludedFromBuild")) {
        file.Excluded.push_back(config_name->value());
        auto pos = std::remove_if(targets.begin(), targets.end(),
        [=](vs::Configuration* ptr)->bool {
          return ptr->Name.compare(config_name->value())==0;
        });

        targets.erase(pos, targets.end());
      }

      // Precompiled only describes the source file (.cpp,cc, etc)
      // that created the compiled header(.pch)
      if(const XMLNode* tool = file_configuration->first_node("Tool")) {

        if(tool->first_attribute("UsePrecompiledHeader")) {
          file.Precompiled = true;
        }

        if(const XMLAttribute* compile_as_c = tool->first_attribute("CompileAs")) {
          file.CompileAsC = strcmp(compile_as_c->value(), "1") == 0;
        }
      }

      file_configuration = file_configuration->next_sibling();
    } //while(file_configuration)


    if(const XMLNode* parent = node->parent()) {
      if(strcmp(parent->name(), "Filter") == 0) {
        //file.Filter.assign(parent->first_attribute("Name")->value());
      }
    }

    // check for forced includes
    foreach(auto& current_config, configs) {
      string forced_includes(current_config.ToolProperties["VCCLCompilerTool"]["ForcedIncludeFiles"]);
      if(!forced_includes.empty()) {
        StringTokenizer tok(forced_includes, ";");
        while(tok.next()) {
          if(file.RelativePath.find(tok.token()) != string::npos) {
            file.ForcedInclude = true;
          }
        }
      }
    }

    // files is reserved, no re-allocations
    // should ever take place
    files->push_back(move(file));
    vs::File* file_ptr = &(*(files->end() - 1));

    for(size_t i = 0, end = targets.size(); i < end; ++i) {
      targets[i]->Files.push_back(file_ptr);
    }
  } //foreach


  return true;
}

#endif
bool VCProjectParser::Files(std::vector<vs::File>* files) {
  if(!root->first_node("Files")) return false;

  vector<const XMLNode*> file_nodes;
  CollectFileNodes(root->first_node("Files")->first_node(), &file_nodes);

  if(file_nodes.empty()) return false;
  // reserve each configuration's file list
  files->reserve(file_nodes.size());
  foreach(const XMLNode* node, file_nodes) {

    const XMLAttribute* attr = node->first_attribute("RelativePath");
    if(!attr) continue;

    vs::File file;
    file.Precompiled = false;
    file.CompileAsC = false;
    file.ForcedInclude = false;
    

    file.RelativePath.assign(attr->value());
    file.AbsolutePath.assign(AbsoluteFilePath(file.RelativePath));
    size_t separator = file.RelativePath.find_last_of("\\/");
    if(separator == string::npos) {
      file.Name.assign(attr->value());
    } else {
      file.Name.assign(file.RelativePath.substr(separator+1));
    }


    // check per-file FileConfiguration
    const XMLNode* file_configuration = node->first_node("FileConfiguration");
    while(file_configuration) {

      // Remove the configuration that matches 'config_name'
      // from the list of targets
      // since the vcfile is ExcludedFromBuild
      const XMLAttribute* config_name = file_configuration->first_attribute("Name");
      if(file_configuration->first_attribute("ExcludedFromBuild")) {
        file.Excluded.push_back(config_name->value());
        //        auto pos = std::remove_if(targets.begin(), targets.end(),
        //    [=](VCConfiguration* ptr)->bool {
        //     return ptr->Name.compare(config_name->value())==0;
        //    });

        // targets.erase(pos, targets.end());
      }

      // Precompiled only describes the source file (.cpp,cc, etc)
      // that created the compiled header(.pch)
      if(const XMLNode* tool = file_configuration->first_node("Tool")) {
        file.Precompiled = tool->first_attribute("UsePrecompiledHeader") != 0;

        if(const XMLAttribute* compile_as_c = tool->first_attribute("CompileAs")) {
          file.CompileAsC = strcmp(compile_as_c->value(), "1") == 0;
        }
      }

      file_configuration = file_configuration->next_sibling();
    } //while(file_configuration)


    if(const XMLNode* parent = node->parent()) {
      if(strcmp(parent->name(), "Filter") == 0) {
//        file.Filter.assign(parent->first_attribute("Name")->value());
      }
    }
    // files is reserved, no re-allocations
    // should ever take place
    files->push_back(move(file));
  } //foreach


  return true;
}

bool VCProjectParser::Parse(char* buffer, size_t len) {
  root = 0;
  macros.clear();
  for(size_t i = 0; i < ARRAY_COUNT(kProjectMacros2008); ++i) {
    macros.insert(make_pair(kProjectMacros2008[i], string("")));
  }

  src.clear();
  src.reserve(len);
  copy(buffer, buffer + len, back_inserter(src));
  src.push_back('\0');

  doc.parse<0>(&src[0]);
  root = doc.first_node("VisualStudioProject");
  
  configurations.clear();
  files.clear();
  
  FilterByNodeName("File",root,files);
  FilterByNodeName("Configuration",root,configurations);

  return root != 0;
}

bool VCProjectParser::Parse(const std::string& path) {
  origin.assign(path);
  string buffer("");
  FileToString(path, &buffer);
  root = 0;
  macros.clear();
  for(size_t i = 0; i < ARRAY_COUNT(kProjectMacros2008); ++i) {
    macros.insert(make_pair(kProjectMacros2008[i], string("")));
  }

  macros["$(ProjectDir)"].assign(path);

  src.clear();
  src.reserve(buffer.length());
  copy(buffer.begin(), buffer.end(), back_inserter(src));
  src.push_back('\0');

  doc.parse<0>(&src[0]);
  root = doc.first_node("VisualStudioProject");

  return root != 0;
}
