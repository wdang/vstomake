
#include "internal.h"

#include <algorithm>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <utility>
#include <string.h>
#include <sys/stat.h>

#include "utility.h"
#include "string_tokenizer.h"
#include "msvc_object_model.h"


using std::make_pair;
using std::string;
using std::ifstream;
using std::vector;
using std::unordered_map;
using std::remove_if;
using std::make_pair;
using std::move;
using std::copy_if;


namespace {
typedef rapidxml::xml_document<>  XMLDocument;
typedef rapidxml::xml_node<>      XMLNode;
typedef rapidxml::xml_attribute<> XMLAttribute;
}

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

typedef std::unordered_map<std::string,std::string> MacroMap;
// In place-expansion of macros(see kProjectMacros) found in input
void ExpandMacros(string* input, MacroMap& macros) {

  for(size_t i = 0; i < ARRAY_COUNT(kProjectMacros); ++i) {
    size_t pos = 0;

    // macros should've been initialized with kProjectMacros
    // so we dont need to check the results of find.
    auto iter = macros.find(kProjectMacros[i]);
    while((pos = input->find(kProjectMacros[i], pos)) != string::npos) {
      input->replace(pos, strlen(kProjectMacros[i]), iter->second);
    }
  }
}

// Expand macros(see kProjectMacros) found in input
//
// @input: target for macro expansion
// @macros: definitions used for macro expansion
//
// Returns a copy of input with macros expanded
string ExpandMacros(const string& input, MacroMap& macros) {
  string results(input);
  ExpandMacros(&results, macros);
  return results;
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

void GetFileCountWorkaround(){
  XMLDocument doc;
   GetFileCount(doc);
}


void FilterCPPSources(const std::vector<VCFile*>& sources, std::vector<VCFile*>* out) {
  // Exclude non c++ source files
  static const char* kCPPExtensions[] = {".cc", ".cpp", ".cxx", ".c++", ".C", ".cp", ".CPP"};
  out->reserve(sources.size());
  copy_if(sources.begin(), sources.end(), std::back_inserter(*out),
  [](VCFile* file)->bool {
    foreach(auto* ext, kCPPExtensions) {
      if(file->RelativePath.find(ext) != string::npos) {
        return true;
      }
    }
    return false;
  });
}