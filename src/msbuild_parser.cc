#include "precompiled.h"
#include "msbuild_parser.h"
#include "utility.h"

#include <functional>

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


namespace {
typedef rapidxml::xml_document<>  XMLDocument;
typedef rapidxml::xml_node<>      XMLNode;
typedef rapidxml::xml_attribute<> XMLAttribute;
}

static const char* const kProjectMacros2010[] = {
  "$(Configuration)",
  "$(DevEnvDir)",
  "$(FrameworkDir)",
  "$(FrameworkSDKDir)",
  "$(FrameworkVersion)",
  "$(FxCopDir)",
  "$(InputDir)",
  "$(InputExt)",
  "$(InputFileName)",
  "$(InputName)",
  "$(InputPath)",
  "$(IntDir)",
  "$(OutDir)",
  "$(ParentName)",
  "$(Platform)",
  "$(ProjectDir)",
  "$(ProjectExt)",
  "$(ProjectFileName)",
  "$(ProjectName)",
  "$(ProjectPath)",
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
  "$(TargetDir)",
  "$(TargetExt)",
  "$(TargetFileName)",
  "$(TargetName)",
  "$(TargetPath)",
  "$(VCInstallDir)",
  "$(VSInstallDir)",
  "$(WebDeployPath)",
  "$(WebDeployRoot)",
};

MSBuildParser::MSBuildParser()
  : doc() {}

bool MSBuildParser::Parse(char* buffer, size_t len) {
  macros.clear();

  for(size_t i = 0; i < ARRAY_COUNT(kProjectMacros2010); ++i) {
    macros.insert(std::make_pair(kProjectMacros2010[i], string("")));
  }
  src.clear();
  src.reserve(len);
  copy(buffer, buffer + len, back_inserter(src));
  src.push_back('\0');

  doc.parse<0>(&src[0]);
  project = doc.first_node("Project");

  return project !=NULL;
}

bool MSBuildParser::Parse( const std::string& ){
  return false;
}

bool MSBuildParser::Configurations(vector<vs::Configuration>* out) {
  if(!project || !project->first_node("ItemGroup"))
    return false;

  //static const int kNameOffset   = sizeof("'$(Configuration)|$(Platform)'=='");
  XMLNode* project_configuration = 0;
  XMLNode* current = project->first_node("ItemGroup");
  while(current) {
    project_configuration = current->first_node("ProjectConfiguration");
    if(project_configuration != 0) { break; }
    current = current->next_sibling();
  }

  function<vs::Configuration*(const char*)> GetVCConfigurationPointer = [=](const char* name)->vs::Configuration* {
    for(size_t i = 0, end = out->size(); i <  end; ++i) {
      if((*out)[i].Name.compare(name) == 0)
        return &(*out)[i];
    }
    return 0;
  };

  while(project_configuration) {
    XMLAttribute* include = project_configuration->first_attribute("Include");
    vs::Configuration config;
    config.Name.assign(include ? include->value() : "");

    XMLNode* node = project_configuration->first_node("Configuration");
    config.ConfigurationName.assign(node ? node->value() : "");

    node = project_configuration->first_node("Platform");
    config.Platform.assign(node ? node->value() : "");

    out->push_back(config);
    project_configuration = project_configuration->next_sibling();
  }


  current = project->first_node("PropertyGroup");
  while(current) {
    if(XMLAttribute* label = current->first_attribute("Label")) {
      if(strcmp(label->value(), "Configuration") == 0) {

        if(current->first_node("ConfigurationType")) {
          // not implemented
        }
        if(current->first_node("CharacterSet")) {
          // not implemented
        }
      }
    }

    current = current->next_sibling();
  }

  return false;
}

bool MSBuildParser::Files(vector<vs::File>* files) {
  if(!project || !project->first_node("ItemGroup"))
    return false;

  XMLNode* item_group = project->first_node("ItemGroup");
  while(item_group) {
    if(XMLNode* cinclude = item_group->first_node("CInclude")) {
      if(XMLAttribute* include = cinclude->first_attribute("Include")) {
        vs::File file;
        file.RelativePath.assign(include->value());
        file.Precompiled = cinclude->first_node("PrecompiledHeader") != 0;
        file.Precompiled = cinclude->first_node("ExcludedFromBuild") != 0;
        files->push_back(file);
      }
    }
    item_group=item_group->next_sibling();
  }

  return true;
}

bool MSBuildParser::Filters(std::vector<vs::Filter>*) {
  return false;
}

bool MSBuildParser::CompilerProperties(const string&, unordered_map<string, string>*) {
  return false;
}

bool MSBuildParser::LibrarianProperties(const string&, unordered_map<string, string>*) {
  return false;
}

bool MSBuildParser::LinkerProperties(const string&, unordered_map<string, string>*) {
  return false;
}

bool MSBuildParser::ProjectProperties(std::unordered_map<std::string, std::string>*) {
  return false;
}
