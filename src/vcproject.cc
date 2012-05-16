// Copyright 2012 William Dang. 
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "precompiled.h"
#include "vcproject.h"
#include "utility.h"

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <utility>

#pragma warning(disable: 4100)

using std::make_pair;
using std::string;
using std::istringstream;
using std::ifstream;
using std::vector;
using std::unordered_map;
using namespace rapidxml;


// Returns the length of the given array.
// compile time assertion with "ERROR_Non_Array_Type"
// if x is not a static array.
#define ARRAY_COUNT(x) (sizeof(ERROR_Non_Array_Type(x)))

namespace {
typedef rapidxml::xml_document<>                     XMLDocument;
typedef rapidxml::xml_node<>                         XMLNode;
typedef rapidxml::xml_attribute<>                    XMLAttribute;
typedef std::vector<rapidxml::xml_node<>*>           XMLNodeList;
typedef std::vector<std::string>                     StringList;
typedef std::unordered_map<std::string, std::string> PropertyMap;
std::unordered_map<std::string, StringList> PropertyMappings;

// ARRAY_COUNT implementation
#if defined(_WIN64) || defined(__LP64__) || defined(_M_AMD64)
template<class ArrayType, size_t N>
char(__unaligned &ERROR_Non_Array_Type(ArrayType(&)[N]))[N];
#endif

// GCC wants both const and non-const versions
#ifndef _MSC_VER
template<class ArrayType, size_t N>
char(&ERROR_Non_Array_Type(const ArrayType(&)[N]))[N];
#endif

template<class ArrayType, size_t N>
char(&ERROR_Non_Array_Type(ArrayType(&)[N]))[N];
}


static unordered_map<string, string> GMacroMap;
static const char* const kProjectMacros[] = {
  "$(ConfigurationName)",
  "$(DevEnvDir)",
  "$(FrameworkDir)",
  "$(FrameworkSDKDir)",
  "$(FrameworkVersion)",
  "$(FxCopDir)"
  "$(Inherit)",
  "$(InputDir)",
  "$(InputExt)",
  "$(InputFileName)",
  "$(InputName)",
  "$(InputPath)",
  "$(IntDir)",
  "$(NoInherit)",
  "$(OutDir)",
  "$(ParentName)",
  "$(PlatformName)",
  "$(ProjectDir)",
  "$(ProjectExt)",
  "$(ProjectFileName)",
  "$(ProjectName)",
  "$(ProjectPath)",
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


// VCProjParser consists of 2 state machines that
// handle node transistions for the following nodes:
// -Configurations
// -Files
//
// The Configurations state machine parses the tool properties
// found in the vcproj file
// see "Interfaces" table on:
// http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.vcprojectengine(VS.90).aspx
#define VCPROJ_TOOL_INTERFACES X(VCALinkTool) \
  X(VCAppVerifierTool) \
  X(VCBscMakeTool) \
  X(VCCLCompilerTool) \
  X(VCCustomBuildTool) \
  X(VCFxCopTool) \
  X(VCLibrarianTool) \
  X(VCLinkerTool) \
  X(VCManagedResourceCompilerTool) \
  X(VCManifestTool) \
  X(VCMidlTool) \
  X(VCNMakeTool) \
  X(VCPostBuildEventTool) \
  X(VCPreBuildEventTool) \
  X(VCPreLinkEventTool) \
  X(VCResourceCompilerTool) \
  X(VCWebDeploymentTool) \
  X(VCWebServiceProxyGeneratorTool) \
  X(VCXDCMakeTool) \
  X(VCXMLDataGeneratorTool) \
  X(Unknown)

// tool interface strings
#define X(NAME) # NAME,
static const char* kToolStrings[] = {VCPROJ_TOOL_INTERFACES};
#undef X

// tool interface types
#define X(NAME) ToolType_ ## NAME,
enum ToolType {VCPROJ_TOOL_INTERFACES};
#undef X

#undef VCPROJ_TOOL_INTERFACES


static ToolType GetToolType(XMLNode* node) {
  if (!node) return ToolType_Unknown;
  ToolType type = ToolType_Unknown;
  if (XMLAttribute* attr = node->first_attribute("Name")) {
    char* name = attr->value();
    for (size_t i = 0; i < ARRAY_COUNT(kToolStrings); ++i) {
      if (strcmp(name, kToolStrings[i]) == 0) {
        type = static_cast<ToolType>(i);
        break;
      }
    }
  }
  return type;
}

static void ParseFileNodes(const XMLDocument& doc, VCProject::Configuration* config) {
  typedef std::vector<XMLNode*> NodeList;

  // recurse into the filter nodes and collect the file nodes
  std::function<void(XMLNode*, NodeList&)> filter_filenodes =
  [&filter_filenodes](XMLNode* node, NodeList& files) {
    if (!node) return;
    if (auto* attr = node->first_attribute("Name")) {
      filter_filenodes(node->first_node(), files);
      filter_filenodes(node->next_sibling(), files);
    }
    else if (auto* attr = node->first_attribute("RelativePath")) {
      files.push_back(node);
      filter_filenodes(node->next_sibling(), files);
    }
  };

  XMLNode* root  = doc.first_node()->first_node("Files");
  XMLNode* start = root->first_node();
  NodeList files;
  filter_filenodes(start, files);
  std::string config_name(config->name);
  config_name.append("|");
  config_name.append(config->platform);


  foreach(auto node, files) {
    if (auto attr = node->first_attribute("RelativePath")) {
      char* current_file_path = attr->value();
      auto* has_config        = node->first_node("FileConfiguration");

      bool file_excluded      = false;

      // check per-file FileConfiguration
      while (has_config) {
        auto* name = has_config->first_attribute("Name");
        if (config_name.compare(name->value())==0) {

          if (auto* excluded = has_config->first_attribute("ExcludedFromBuild")) {
            config->excluded_files.push_back(current_file_path);
            file_excluded = true;
            break;
          }

        }
        has_config = has_config->next_sibling();
      }


      if (!file_excluded) {
        VCProject::File file;
        file.path = current_file_path;
        config->files.push_back(file);
      }
    }
  }
}

// for each configuration,
// parse tool properties
// and collect files referenced in the vcproj
static void ParseConfigurations(const XMLDocument& doc, VCProject* out) {
  XMLNode* configurations = doc.first_node()->first_node("Configurations");
  XMLNode* root           = doc.first_node("VisualStudioProject");

  // populate/reset the macro map for substitutions
  for (size_t i = 0; i < ARRAY_COUNT(kProjectMacros); ++i) {
    GMacroMap[kProjectMacros[i]] = std::string("");
  }

  if (XMLAttribute* attr = root->first_attribute("Name")) {
    out->name                   = attr->value();
    GMacroMap["$(InputName)"]   = out->name;
    GMacroMap["$(ProjectName)"] = out->name;
    GMacroMap["$(TargetName)"]  = out->name;
  }

  if (XMLAttribute* attr = root->first_attribute("ProjectGUID")) {
    out->guid = attr->value();
  }


  XMLNode* configuration = configurations->first_node();
  while (configuration) {
    VCProject::Configuration config;
    //ProjectConfiguration config;
    if (XMLAttribute* attr = configuration->first_attribute("Name")) {
      size_t len = strcspn(attr->value(), "|");
      config.name.assign(attr->value(), len);
      config.platform.assign(attr->value()+len+1);
      GMacroMap["$(ConfigurationName)"] = config.name;
    }

    if (XMLAttribute* attr = configuration->first_attribute("OutputDirectory")) {
      config.output_dir.assign(attr->value());


      for (size_t i = 0; i < ARRAY_COUNT(kProjectMacros); ++i) {
        Replace(&config.output_dir, kProjectMacros[i], GMacroMap[kProjectMacros[i]]);
        Replace(&config.intermediate_dir, kProjectMacros[i], GMacroMap[kProjectMacros[i]]);
      }
      GMacroMap["$(OutDir)"] = config.output_dir;
    }

    if (XMLAttribute* attr = configuration->first_attribute("IntermediateDirectory")) {
      config.intermediate_dir.assign(attr->value());

      for (size_t i = 0; i < ARRAY_COUNT(kProjectMacros); ++i) {
        Replace(&config.intermediate_dir, kProjectMacros[i], GMacroMap[kProjectMacros[i]]);
      }
      GMacroMap["$(IntDir)"] = config.intermediate_dir;
    }

    if (XMLAttribute* attr = configuration->first_attribute("InheritedPropertySheets")) {
      config.vsprops.assign(attr->value());
    }

    if (XMLAttribute* attr = configuration->first_attribute("ConfigurationType")) {
      config.type = (VCProject::Type)strtol(attr->value(), 0, 10);
    }

    //collect tool properties
    XMLNode* tool = configuration->first_node("Tool");    
    while (tool) {
      XMLAttribute* attr = tool->first_attribute("Name");
      char* toolname     = attr->value();
      
      while (attr = attr->next_attribute()) {
        string value(attr->value());
        
        // substitute project macros
        for (size_t i = 0; i < ARRAY_COUNT(kProjectMacros); ++i) {
          Replace(&value, kProjectMacros[i], GMacroMap[kProjectMacros[i]]);
        }
        
        
        config.properties[toolname].insert(make_pair(attr->name(), value));
      }
      tool = tool->next_sibling();
    }


    ParseFileNodes(doc, &config);
    out->configurations.push_back(config);
    configuration = configuration->next_sibling();
  }
}

VCProject::VCProject(const std::string& contents)
  : guid(""), name(""), path(""), version("") {
  XMLDocument doc;
  doc.parse<0>(const_cast<char*>(contents.c_str()));
  ParseConfigurations(doc, this);
}
