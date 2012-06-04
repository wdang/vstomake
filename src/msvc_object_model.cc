#include "msvc_object_model.h"

#include <algorithm>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <utility>

#include  "utility.h"

using std::make_pair;
using std::string;
using std::ifstream;
using std::vector;
using std::unordered_map;
using std::remove_if;
using std::make_pair;

namespace {
typedef rapidxml::xml_document<>  XMLDocument;
typedef rapidxml::xml_node<>      XMLNode;
typedef rapidxml::xml_attribute<> XMLAttribute;
}

// MacroMap holds the current state of project build macros.
typedef std::unordered_map<std::string, std::string> MacroMap;

// @@DummyMacros is used as a default argument for functions that
// take a reference to a MacroMap
static MacroMap DummyMacros;

// @@kProjectMacros is used to initialize MacroMap's keys.
// Documentation from
// http://msdn.microsoft.com/en-us/library/c02as0cs(VS.90).aspx
static const char* const kProjectMacros[] = {
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


// @@kVCToolInterfaces is used to initialize VCConfiguration's ToolProperties.
static const char* kVCToolInterfaces[]={
  "VCALinkTool",
  "VCAppVerifierTool",
  "VCBscMakeTool",
  "VCCLCompilerTool",
  "VCCustomBuildTool",
  "VCFxCopTool",
  "VCLibrarianTool",
  "VCLinkerTool",
  "VCManagedResourceCompilerTool",
  "VCManifestTool",
  "VCMidlTool",
  "VCNMakeTool",
  "VCPostBuildEventTool",
  "VCPreBuildEventTool",
  "VCPreLinkEventTool",
  "VCResourceCompilerTool",
  "VCWebDeploymentTool",
  "VCWebServiceProxyGeneratorTool",
  "VCXDCMakeTool",
  "VCXMLDataGeneratorTool"
};

// Expand macros(see kProjectMacros) found in input
//
// @input: target for macro expansion
// @macros: definitions used for macro expansion
//
// Returns a copy of input with macros expanded
std::string ExpandMacros(const std::string& input, MacroMap& macros = DummyMacros) {

  std::string results(input);
  for(size_t i = 0; i < ARRAY_COUNT(kProjectMacros); ++i) {
    size_t pos = 0;

    // macros should've been initialized with kProjectMacros
    // so we dont need to check the results of find.
    auto iter = macros.find(kProjectMacros[i]);
    while((pos = input.find(kProjectMacros[i], pos)) != std::string::npos) {
      results.replace(pos, strlen(kProjectMacros[i]), iter->second);
    }
  }

  return results;
}

// In place-expansion of macros(see kProjectMacros) found in input
void ExpandMacros(std::string* input, MacroMap& macros = DummyMacros) {

  for(size_t i = 0; i < ARRAY_COUNT(kProjectMacros); ++i) {
    size_t pos = 0;

    // macros should've been initialized with kProjectMacros
    // so we dont need to check the results of find.
    auto iter = macros.find(kProjectMacros[i]);
    while((pos = input->find(kProjectMacros[i], pos)) != std::string::npos) {
      input->replace(pos, strlen(kProjectMacros[i]), iter->second);
    }
  }

}


void InitializeMacroMap(MacroMap* macros) {
  for(size_t i = 0; i < ARRAY_COUNT(kProjectMacros); ++i) {
    macros->insert(std::make_pair(std::string(kProjectMacros[i]), std::string("")));
  }
}

// Number of referenced files in the given vcproj file.
//
// @doc: .vcproj XMLDocument
//
// Returns zero on error.
static size_t GetFileCount(const XMLDocument& doc) {
  XMLNode* root = doc.first_node("VisualStudioProject");
  if(!root || !root->first_node("Files")) return 0;


  std::function<void(XMLNode*, size_t&)> traverse_nodes =
  [&traverse_nodes](XMLNode* node, size_t& count) {
    if(!node) return;
    // we id File nodes by the existance of the RelativePath attribute
    // we id filter nodes by the existance of the Name attribute
    if(auto* attr = node->first_attribute("Name")) {
      traverse_nodes(node->first_node(), count);
      traverse_nodes(node->next_sibling(), count);
    } else if(auto* attr = node->first_attribute("RelativePath")) {
      ++count;
      traverse_nodes(node->next_sibling(), count);
    }
  };

  size_t count   = 0;
  XMLNode* files = root->first_node("Files");
  traverse_nodes(files->first_node(), count);
  return count;
}

// Collect property values in the "VisualStudioProject"
// node of the vcproj
void ParseProjectProperties(const XMLDocument& doc, VCProject* p, MacroMap& macros= DummyMacros) {
  XMLNode* root = doc.first_node("VisualStudioProject");

  if(XMLAttribute* attr = root->first_attribute("Name")) {
    p->Name.assign(attr->value());
    macros["$(InputName)"]   = attr->value();
    macros["$(ProjectName)"] = attr->value();
    macros["$(TargetName)"]  = attr->value();
  }

  if(XMLAttribute* attr = root->first_attribute("ProjectType")) {
    p->ProjectType.assign(attr->value());
  }

  if(XMLAttribute* attr = root->first_attribute("Version")) {
    p->Version.assign(attr->value());
  }

  if(XMLAttribute* attr = root->first_attribute("ProjectGUID")) {
    p->ProjectGUID.assign(attr->value());
  }

  if(XMLAttribute* attr = root->first_attribute("RootNamespace")) {
    p->RootNamespace.assign(attr->value());
  }

  if(XMLAttribute* attr = root->first_attribute("Keyword")) {
    p->Keyword.assign(attr->value());
  }
}

// Extract all platform names referenced in the project
void ParsePlatforms(const XMLDocument& doc, std::vector<std::string>* platforms,MacroMap& macros= DummyMacros) {
  XMLNode* platform = doc.first_node("VisualStudioProject")->first_node("Platforms");
  platform = platform->first_node("Platform");
  while(platform) {
    if(XMLAttribute* attr = platform->first_attribute("Name")) {
      platforms->push_back(attr->value());
    }
    platform = platform->next_sibling();
  }
}
#ifdef _MSC_VER
    //C4706: assignment within conditional expression
#pragma warning(push)
#pragma warning(disable: 4706)
#endif

// Extract all tool properties and their values
// in the given "Configuration" node
static void CollectToolProperties(XMLNode* configuration, unordered_map<string, unordered_map<string, string> >* out,
                                  MacroMap& macros= DummyMacros) {
  XMLNode* tool = configuration->first_node("Tool");
  unordered_map<string, unordered_map<string, string> >& ref = *out;
  while(tool) {
    XMLAttribute* attr = tool->first_attribute("Name");
    const char* toolname     = attr->value();
    while(attr = attr->next_attribute()) {
      string value(attr->value());
      ExpandMacros(&value,macros);
      ref[toolname].insert(make_pair(attr->name(), value));
    }
    tool = tool->next_sibling();
  }
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif


// Traverse all xml Filter and File nodes and
// return a flat structure the File nodes
static void CollectFileNodes(XMLNode* node, std::vector<XMLNode*>* out) {
  if(!node) return;

  // id File nodes by the existance of a RelativePath attribute
  // id Filter nodes by the existance of a Name attribute
  if(auto* attr = node->first_attribute("Name")) {
    CollectFileNodes(node->first_node(), out);
    CollectFileNodes(node->next_sibling(), out);
  } else if(auto* attr = node->first_attribute("RelativePath")) {
    out->push_back(node);
    CollectFileNodes(node->next_sibling(), out);
  }
}


// Extract all properties and values
// of all "Configuration" nodes in the project
void ParseConfigurations(const XMLDocument& doc, std::vector<VCConfiguration>* configs,
                         MacroMap& macros = DummyMacros) {
  XMLNode* root = doc.first_node("VisualStudioProject");
  if(!root || !root->first_node("Configurations")) return;

  XMLNode* configurations = root->first_node("Configurations");
  XMLNode* node           = configurations->first_node();
  while(node) {
    string output_dir("");
    string intermediate_dir("");
    string property_sheets("");
    
    VCConfiguration config;
    
    // initialize all tool properties
    
    if(XMLAttribute* attr = node->first_attribute("Name")) {
      config.Name.assign(attr->value());
      config.ConfigurationName.assign(config.Name.substr(0, config.Name.find_first_of('|')));
      config.Platform.assign(config.Name.substr(config.Name.find_first_of('|')+1));

      macros["$(ConfigurationName)"] = config.ConfigurationName;
      macros["$(PlatformName)"]      = config.Platform;
    }



    if(XMLAttribute* attr = node->first_attribute("OutputDirectory")) {
      output_dir.assign(attr->value());
      ExpandMacros(&output_dir,macros);
      macros["$(OutDir)"] = output_dir;
    }


    if(XMLAttribute* attr = node->first_attribute("IntermediateDirectory")) {
      intermediate_dir.assign(attr->value());
      ExpandMacros(&intermediate_dir,macros);
      macros["$(IntDir)"] = intermediate_dir;
    }


    if(XMLAttribute* attr = node->first_attribute("InheritedPropertySheets")) {
      property_sheets.assign(attr->value());;
    }


    if(XMLAttribute* attr = node->first_attribute("ConfigurationType")) {
      config.ConfigurationType = (VCConfiguration::Type)strtol(attr->value(), 0, 10);
    }

    ExpandMacros(&output_dir,macros);
    ExpandMacros(&intermediate_dir,macros);
    ExpandMacros(&property_sheets,macros);
    
    config.IntermediateDirectory = intermediate_dir;
    config.OutputDirectory       = output_dir;
    config.PropertySheets        = property_sheets;
    
    
    CollectToolProperties(node, &config.ToolProperties,macros);
    
    
    
    configs->push_back(config);    
    node = node->next_sibling();
  }
}

// Traverse all xml Filter and File nodes in the given vcproj
// and populate the given vectors
//
// There are two return values: 'config' and 'project_files'
// After the call to ParseFiles():
//  -- 'project_files' will contain ALL files referenced in the vcproj(included and excluded from build)
//  -- In 'configs', the VCConfiguration's referenced list of VCFiles will only
//     those that are not marked with ExcludedFromBuild
static void ParseFiles(const XMLDocument& doc, std::vector<VCConfiguration>* configs,
                       std::vector<VCFile>* project_files,MacroMap& macros = DummyMacros) {
  typedef std::vector<XMLNode*> NodeList;

  // collect a flat file list of nodes
  if(!doc.first_node() || !doc.first_node()->first_node("Files"))
    return;

  XMLNode* root  = doc.first_node()->first_node("Files");
  XMLNode* start = root->first_node();

  NodeList files;
  CollectFileNodes(start, &files);

  project_files->reserve(files.size());
  std::vector<VCConfiguration>& ref = *configs;

  // reserve each configuration's file list
  for(size_t i = 0, end = ref.size(); i < end; ++i) {
    ref[i].Files.reserve(files.size());
  }


  foreach(auto file_node, files) {
    auto attr = file_node->first_attribute("RelativePath");

    if(!attr) continue;

    // build a list of configurations that will reference the VCFile
    std::vector<VCConfiguration*> target_configs;
    for(size_t i = 0, end = ref.size(); i < end; ++i) {
      target_configs.push_back(&ref[i]);
    }

    VCFile file;
    memset(&file, 0, sizeof(file));

    file.RelativePath.assign(attr->value());
    file.AbsolutePath.assign(AbsoluteFilePath(file.RelativePath));

    // check per-file FileConfiguration
    auto* file_configuration = file_node->first_node("FileConfiguration");
    while(file_configuration) {

      // If the file is marked as ExcludedFromBuild don't add it
      // to the specific build configuration's referenced list of files
      auto* config_name = file_configuration->first_attribute("Name");
      if(auto* excluded = file_configuration->first_attribute("ExcludedFromBuild")) {
        file.ExcludedFromBuild = true;

        auto pos = std::remove_if(target_configs.begin(), target_configs.end(),
        [=](VCConfiguration* ptr)->bool {
          return ptr->Name.compare(config_name->value())==0;
        });

        target_configs.erase(pos, target_configs.end());
      }

      // PrecompiledHeader only describes the source file (.cpp,cc, etc)
      // that creates the compiled header, rather than the header itself.
      if(auto* tool = file_configuration->first_node("Tool")) {
        if(auto* use_precompiled_header = tool->first_attribute("UsePrecompiledHeader")) {
          file.PrecompiledHeader = true;
        }


        if(auto* compile_as_c = tool->first_attribute("CompileAs")) {
          file.CompileAsC = strcmp(compile_as_c->value(), "1") == 0;
        }
      }

      file_configuration = file_configuration->next_sibling();
    }


    if(auto parent = file_node->parent()) {
      if(strcmp(parent->name(), "Filter") == 0) {
        file.Filter.assign(parent->first_attribute("Name")->value());
      }
    }

    // project_files is reserved, no re-allocations
    // should ever take place
    project_files->push_back(file);
    VCFile* file_ptr = &(*(project_files->end() - 1));

    for(size_t i = 0, end = target_configs.size(); i < end; ++i) {
      target_configs[i]->Files.push_back(file_ptr);
    }
  } //foreach
}

// Parse the entire vcproj document
// and populate the given project that pointer is pointing to
void ParseVCProject(XMLDocument& doc, VCProject* project) {

  MacroMap macros;
  InitializeMacroMap(&macros);

  ParseProjectProperties(doc, project,macros);
  ParsePlatforms(doc, &project->Platforms,macros);
  ParseConfigurations(doc, &project->Configurations,macros);

  size_t file_count = GetFileCount(doc);
  project->Files.reserve(file_count);

  foreach(auto& config, project->Configurations) {
    config.Files.reserve(file_count);
  }

  ParseFiles(doc, &project->Configurations, &project->Files,macros);
}

VCProject::VCProject(const std::string& path, int* errors /*= 0*/)
:Path(path){
  int status;
  if(!errors) {
    status = 0;
    errors = &status;
  }

  struct stat s;
  if(stat(Path.c_str(), &s) == -1) {
    //file does not exist
    *errors = 1;
    return;
  }

  std::string contents("");
  FileToString(Path, &contents);
  if(!contents.size()) {
    *errors = 1;
    return;
  }
  
  AbsolutePath.assign(AbsoluteFilePath(Path)); 
  
  size_t separator = AbsolutePath.find_last_of("\\/");
  ProjectFile.assign(AbsolutePath.substr(separator+1));
  ProjectDirectory.assign(AbsolutePath.substr(0,separator));
  
  XMLDocument doc;
  doc.parse<0>(&contents[0]);  
  ParseVCProject(doc,this);
  
  // see C++11 standard: § 23.2.5 for reallocation 
  // policy of and unordered_map
  for (size_t i = 0,end = Configurations.size(); i < end ; ++i){
    Configurations[i].CLCompilerTool.properties = &Configurations[i].ToolProperties.find("VCCLCompilerTool")->second;
  }
  
}