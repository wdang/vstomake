#include "precompiled.h"
#include "vcproj_parser.h"
#include "utility.h"
#include "string_tokenizer.h"

#ifdef _MSC_VER
//#pragma warning(push)
//#pragma warning(disable: 4706)
#endif

using std::function;
using std::make_pair;
using std::string;
using std::ifstream;
using std::vector;
using std::unordered_map;
using std::remove_if;
using std::make_pair;
using std::move;
using std::back_inserter;
using std::exception;
using std::copy;
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


// Traverse all 'Filter' and 'File' nodes and
// return a flat structure containing only "File" nodes
static void CollectFileNodes(const XMLNode* node, std::vector<const XMLNode*>* out) {
  if(!node) return;
  // id File nodes by the existance of a 'RelativePath' attribute
  // id Filter nodes by the existance of a 'Name' attribute
  if(const XMLAttribute* attr = node->first_attribute("Name")) {
    CollectFileNodes(node->first_node(), out);
    CollectFileNodes(node->next_sibling(), out);
  } else if(const XMLAttribute* attr = node->first_attribute("RelativePath")) {
    out->push_back(node);
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

// Returns a map of all tool properties identified by toolname
// for the specified configuration 'name'
bool GetToolPropertiesForConfiguration(const string& name, const string& toolname,
                                       XMLNode* configurations, unordered_map<string, string>* props) {
  if(!configurations->first_node("Configuration"))
    return false;

  XMLNode* configuration = configurations->first_node("Configuration");

  // find matching configuration
  while(configuration) {    
    XMLAttribute* attr = configuration->first_attribute("Name");
    if(attr && name.compare(attr->value()) == 0) { 
      break;
    }
    configuration = configuration->next_sibling();
  }

  if(!configuration)
    return false;

  XMLNode* tool = configuration->first_node("Tool");
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

VcprojParser::VcprojParser()
  : root(0) {
  for(size_t i = 0; i < ARRAY_COUNT(kProjectMacros2008); ++i) {
    macros.insert(make_pair(kProjectMacros2008[i], ""));
  }
}

bool VcprojParser::ProjectProperties(unordered_map<string, string>* props) {

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
  macros["$(InputName)"]   = (*props)["Name"];
  macros["$(ProjectName)"] = (*props)["Name"];
  macros["$(TargetName)"]  = (*props)["Name"];
  return true;
}

bool VcprojParser::CompilerProperties(const string& config,
                                      unordered_map<string, string>* props) {
  if(!root->first_node("Configurations"))
    return false;
  XMLNode* configurations = root->first_node("Configurations");
  return GetToolPropertiesForConfiguration(config, "VCCLcompilerTool", configurations, props);
}

bool VcprojParser::LibrarianProperties(const string& config,
                                       unordered_map<string, string>* props) {
  if(!root->first_node("Configurations"))
    return false;
  XMLNode* configurations = root->first_node("Configurations");
  return GetToolPropertiesForConfiguration(config, "VCLibrarianTool", configurations, props);
}

bool VcprojParser::LinkerProperties(const string& config,
                                    unordered_map<string, string>* props) {
  if(!root->first_node("Configurations"))
    return false;
  XMLNode* configurations = root->first_node("Configurations");
  return GetToolPropertiesForConfiguration(config, "VCLinkerTool", configurations, props);
}

bool VcprojParser::Configurations(vector<VCConfiguration>* configurations) {
  if(!root->first_node("Configurations"))
    return false;
  
  XMLNode* node = root->first_node("Configurations")->first_node();

  while(node) {
    string output_dir("");
    string intermediate_dir("");
    string property_sheets("");

    VCConfiguration config;

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
      config.ConfigurationType = (VCConfiguration::Type)strtol(attr->value(), 0, 10);
    }

    ExpandMacros(&output_dir, macros);
    ExpandMacros(&intermediate_dir, macros);
    ExpandMacros(&property_sheets, macros);

    config.IntermediateDirectory.assign(intermediate_dir);
    config.OutputDirectory.assign(output_dir);
    config.PropertySheets.assign(property_sheets);

    // Collect tool properties
    XMLNode* tool = node->first_node("Tool");
    while(tool) {
      XMLAttribute* attr   = tool->first_attribute("Name");
      const char* toolname = attr->value();
      attr = attr->next_attribute();
      while(attr) {
        string value(attr->value());
        ExpandMacros(&value, macros);
        config.ToolProperties[toolname].insert(make_pair(attr->name(), value));
        attr = attr->next_attribute();
      }
      tool = tool->next_sibling();
    }

    configurations->push_back(move(config));
    node = node->next_sibling();
  }


  return true;
}

bool VcprojParser::Filters(vector<VCFilter>*) {
  return false;
}

bool VcprojParser::Files(vector<VCConfiguration>* configurations, vector<VCFile>* files) {
  if(!root->first_node("Files")) return false;

  vector<const XMLNode*> file_nodes;
  CollectFileNodes(root->first_node("Files")->first_node(), &file_nodes);

  if(file_nodes.empty()) return false;

  // alternative name for easier access to operator[]
  vector<VCConfiguration>& configs = *configurations;


  // reserve each configuration's file list
  files->reserve(file_nodes.size());
  for(size_t i = 0, end = configs.size(); i < end; ++i) {
    configs[i].Files.reserve(file_nodes.size());
  }

  foreach(const XMLNode* node, file_nodes) {

    const XMLAttribute* attr = node->first_attribute("RelativePath");
    if(!attr) continue;

    VCFile file;
    // build a list of configurations that will
    // reference the VCFile
    vector<VCConfiguration*> targets;
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
      if(const XMLAttribute* excluded = file_configuration->first_attribute("ExcludedFromBuild")) {
        file.Excluded.push_back(config_name->value());
        auto pos = std::remove_if(targets.begin(), targets.end(),
        [=](VCConfiguration* ptr)->bool {
          return ptr->Name.compare(config_name->value())==0;
        });

        targets.erase(pos, targets.end());
      }

      // PrecompiledHeader only describes the source file (.cpp,cc, etc)
      // that created the compiled header(.pch)
      if(const XMLNode* tool = file_configuration->first_node("Tool")) {

        if(auto* use_precompiled_header = tool->first_attribute("UsePrecompiledHeader")) {
          file.PrecompiledHeader = true;
        }

        if(const XMLAttribute* compile_as_c = tool->first_attribute("CompileAs")) {
          file.CompileAsC = strcmp(compile_as_c->value(), "1") == 0;
        }
      }

      file_configuration = file_configuration->next_sibling();
    } //while(file_configuration)


    if(const XMLNode* parent = node->parent()) {
      if(strcmp(parent->name(), "Filter") == 0) {
        file.Filter.assign(parent->first_attribute("Name")->value());
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
    VCFile* file_ptr = &(*(files->end() - 1));

    for(size_t i = 0, end = targets.size(); i < end; ++i) {
      targets[i]->Files.push_back(file_ptr);
    }
  } //foreach


  return true;
}

bool VcprojParser::Files(std::vector<VCFile>* files) {
  if(!root->first_node("Files")) return false;

  vector<const XMLNode*> file_nodes;
  CollectFileNodes(root->first_node("Files")->first_node(), &file_nodes);

  if(file_nodes.empty()) return false;
  // reserve each configuration's file list
  files->reserve(file_nodes.size());
  foreach(const XMLNode* node, file_nodes) {

    const XMLAttribute* attr = node->first_attribute("RelativePath");
    if(!attr) continue;

    VCFile file;

    file.RelativePath.assign(attr->value());
    file.AbsolutePath.assign(AbsoluteFilePath(file.RelativePath));
    size_t separator = file.RelativePath.find_last_of("\\/");
    if(separator == string::npos) {
      file.Filename.assign(attr->value());
    } else {
      file.Filename.assign(file.RelativePath.substr(separator+1));
    }


    // check per-file FileConfiguration
    const XMLNode* file_configuration = node->first_node("FileConfiguration");
    while(file_configuration) {

      // Remove the configuration that matches 'config_name'
      // from the list of targets
      // since the vcfile is ExcludedFromBuild
      const XMLAttribute* config_name = file_configuration->first_attribute("Name");
      if(const XMLAttribute* excluded = file_configuration->first_attribute("ExcludedFromBuild")) {
        file.Excluded.push_back(config_name->value());
        //        auto pos = std::remove_if(targets.begin(), targets.end(),
        //    [=](VCConfiguration* ptr)->bool {
        //     return ptr->Name.compare(config_name->value())==0;
        //    });

        // targets.erase(pos, targets.end());
      }

      // PrecompiledHeader only describes the source file (.cpp,cc, etc)
      // that created the compiled header(.pch)
      if(const XMLNode* tool = file_configuration->first_node("Tool")) {

        if(auto* use_precompiled_header = tool->first_attribute("UsePrecompiledHeader")) {
          file.PrecompiledHeader = true;
        }

        if(const XMLAttribute* compile_as_c = tool->first_attribute("CompileAs")) {
          file.CompileAsC = strcmp(compile_as_c->value(), "1") == 0;
        }
      }

      file_configuration = file_configuration->next_sibling();
    } //while(file_configuration)


    if(const XMLNode* parent = node->parent()) {
      if(strcmp(parent->name(), "Filter") == 0) {
        file.Filter.assign(parent->first_attribute("Name")->value());
      }
    }
    // files is reserved, no re-allocations
    // should ever take place
    files->push_back(move(file));
  } //foreach


  return true;
}

bool VcprojParser::Parse(char* buffer, size_t len) {
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

  return root != 0;
}
#ifdef VSTOMAKE_RUN_TESTS
#include <gtest/gtest.h>
#include <fstream>
namespace {

// VCProjectTest Test fixture.
class TestVcprojParser : public ::testing::Test {
protected:

  // Initialize static data.
  static void SetUpTestCase() {

    ifstream fs("testing\\base.vcproj",
                std::ios::in | std::ios::binary | std::ios::ate);

    const int64_t size = fs.tellg();
    ASSERT_TRUE(size  != -1);
    ASSERT_TRUE(fs.is_open());
    for(size_t i = 0; i < size; ++i) {
      Contents.push_back('\0');
    }

    fs.seekg(0, std::ios::beg);
    fs.read(&Contents[0], Contents.size());
    fs.close();
  }

  // Set-up work for each test.
  TestVcprojParser() {}

  // Called before each test(after the constructor).
  virtual void SetUp() {}

  // Called after each test(before the destructor).
  virtual void TearDown() {}

  // no throw
  virtual ~TestVcprojParser() {}

  // Cleanup static data.
  static void TearDownTestCase() {}

  static string Contents;
  static VcprojParser Parser;
};

string TestVcprojParser::Contents("");
VcprojParser TestVcprojParser::Parser;

TEST_F(TestVcprojParser, Parse) {
  EXPECT_TRUE(Parser.Parse(&Contents[0], Contents.size()));

  //EXPECT_EQ(config.Files.size(), 327);
  //EXPECT_EQ(project.Files.size(),446);
}

TEST_F(TestVcprojParser, Configurations) {
  EXPECT_TRUE(Parser.Parse(&Contents[0], Contents.size()));
  
  vector<VCConfiguration> configs;
  EXPECT_TRUE(Parser.Configurations(&configs));
  
  EXPECT_EQ(configs.size(), 4);
}

TEST_F(TestVcprojParser, Files) {
  EXPECT_TRUE(Parser.Parse(&Contents[0], Contents.size()));
  vector<VCConfiguration> configs;
  EXPECT_TRUE(Parser.Configurations(&configs));

  vector<VCFile> files;
  EXPECT_TRUE(Parser.Files(&configs, &files));
  //EXPECT_EQ(config.Files.size(), 327);
  EXPECT_EQ(files.size(), 446);
}

TEST_F(TestVcprojParser, Filters) {
  EXPECT_TRUE(Parser.Parse(&Contents[0], Contents.size()));

  //EXPECT_EQ(config.Files.size(), 327);
  //EXPECT_EQ(project.Files.size(),446);
}

TEST_F(TestVcprojParser, ProjectProperties) {
  EXPECT_TRUE(Parser.Parse(&Contents[0], Contents.size()));
  unordered_map<string, string> props;
  Parser.ProjectProperties(&props);

  //EXPECT_EQ(config.Files.size(), 327);
  //EXPECT_EQ(project.Files.size(),446);
}


TEST_F(TestVcprojParser, Other) {
  VCProject proj("testing\\base.vcproj");

  //EXPECT_EQ(config.Files.size(), 327);
  //EXPECT_EQ(project.Files.size(),446);
}
} //namespace
#endif
