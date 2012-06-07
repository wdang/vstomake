#include "msvc_object_model.h"

#include <algorithm>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <utility>
#include <string.h>
#include <sys/stat.h>

#include "utility.h"
#include "string_tokenizer.h"

#include "vcproj_parser.h"
#include "vcxproj_parser.h"


using std::make_pair;
using std::string;
using std::ifstream;
using std::vector;
using std::unordered_map;
using std::remove_if;
using std::make_pair;
using std::move;



static const char* const kToolNames[] = {
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

static bool LoadProjectFile(const string& path, string* out, string* errors) {
  struct stat s;
  if(stat(path.c_str(), &s) == -1) {
    //file does not exist
    errors->append(path);
    errors->append(" does not exist");
    return false;
  }
  FileToString(path, out);
  if(!out->size()) {
    errors->append("Failed parsing contents of ");
    errors->append(path);
    return false;
  }
  return true;
}

void DoParse(VCProject* project, ProjectParser* parser,
             vector<unordered_map<string, string> >* tools, string* errors) {
  std::unordered_map<string, string> props;

  if(!parser->ProjectProperties(&props)) {
    errors->append(project->Path);
    errors->append(" does not contains any global properties.");
    return;
  }



  if(!parser->Configurations(&project->Configurations)) {
    errors->append(project->Path);
    errors->append(" does not contains any configurations.");
    return;
  }

  parser->Files(&project->Files);
  foreach(auto& c, project->Configurations) {
    string forced_includes(c.ToolProperties["VCCLCompilerTool"]["ForcedIncludeFiles"]);



    foreach(auto& file, project->Files) {

      auto iter = std::find(file.Excluded.begin(), file.Excluded.end(), c.Name);

      if(iter == file.Excluded.end()) {
        c.Files.push_back(&file);
      }

      if(!forced_includes.empty()) {
        StringTokenizer tok(forced_includes, ";");
        while(tok.next()) {
          if(file.RelativePath.find(tok.token()) != string::npos) {
            file.ForcedInclude = true;
          }
        }
      }
    }
  }

  //  bool process_files = parser->Files(&project->Configurations, &project->Files);
  unordered_map<string, string> tool_props;
  for(size_t i = 0, end = project->Configurations.size(); i < end; ++i) {

    if(parser->CompilerProperties(project->Configurations[i].Name, &tool_props)) {
      tools->push_back(std::move(tool_props));
      //project->Configurations[i].CLCompilerTool.properties = &(*(tools->end()-1));
      tool_props.clear();
    }

    if(parser->LinkerProperties(project->Configurations[i].Name, &tool_props)) {
      tools->push_back(std::move(tool_props));
      tool_props.clear();
    }

    if(parser->LibrarianProperties(project->Configurations[i].Name, &tool_props)) {
      tools->push_back(std::move(tool_props));
      tool_props.clear();
    }
  }
}

enum Encoding {
  Encoding_ASCII,
  Encoding_UTF8,
  Encoding_Unicode,
};

VCProject::VCProject(const string& path, string* errors /*= 0*/)
  : Path(path) {

  string status("");
  if(!errors) {
    errors = &status;
  }

  ProjectParser* parser = nullptr;
  VcprojParser vcproj;
  VcxprojParser vcxproj;

  if(path.find(".vcproj")) {
    parser = &vcproj;
  } else if(path.find(".vcxproj")) {
    parser = &vcxproj;
  }

  if(parser == nullptr) {
    errors->append("Project file not recognized: ");
    errors->append(path);
    return;
  }

  AbsolutePath.assign(AbsoluteFilePath(Path));
  size_t separator = AbsolutePath.find_last_of("\\/");
  ProjectFile      = AbsolutePath.substr(separator+1);
  ProjectDirectory = AbsolutePath.substr(0, separator);


  string buffer("");
  if(!LoadProjectFile(path, &buffer, errors)) {
    return;
  }

  if(parser->Parse(&buffer[0], buffer.size())) {
    std::unordered_map<string, string> props;

    if(!parser->ProjectProperties(&props)) {
      errors->append(Path);
      errors->append(" does not contains any global properties.");
      return;
    }

    Name.assign(props["Name"]);
    ProjectType.assign(props["ProjectType"]);
    RootNamespace.assign(props["RootNamespace"]);
    ProjectGUID.assign(props["ProjectGUID"]);
    Keyword.assign(props["Keyword"]);
    Version.assign(props["Version"]);


    if(!parser->Configurations(&Configurations)) {
      errors->append(Path);
      errors->append(" does not contains any configurations.");
      return;
    }


    if(!parser->Files(&Files)) {
      errors->append(Path);
      errors->append(" does not contains any files.\n");
      // continue here
    }


    foreach(auto& c, Configurations) {
    
      // assign tool pointers
      auto& cltool = c.ToolProperties["VCCLCompilerTool"];
      c.CLCompilerTool.properties = &cltool;

      string forced_includes(cltool["ForcedIncludeFiles"]);

      foreach(auto& file, Files) {
        auto iter = std::find(file.Excluded.begin(), file.Excluded.end(), c.Name);

        if(iter == file.Excluded.end()) {
          c.Files.push_back(&file);
        }

        if(forced_includes.empty())
          continue;

        StringTokenizer tok(forced_includes, ";");
        while(tok.next()) {
          file.ForcedInclude = file.RelativePath.find(tok.token()) != string::npos;
        }
      }
    }
  }
}

VCProject::VCProject(const string& path, ProjectParser* parser /*=0*/, string* errors /*= 0*/) {
  string status("");
  if(!errors) {
    errors = &status;
  }

  AbsolutePath.assign(AbsoluteFilePath(Path));
  size_t separator = AbsolutePath.find_last_of("\\/");
  ProjectFile      = AbsolutePath.substr(separator+1);
  ProjectDirectory = AbsolutePath.substr(0, separator);

  string buffer("");
  if(!LoadProjectFile(path, &buffer, errors)) {
    return;
  }

  if(parser->Parse(&buffer[0], buffer.size())) {}
}

VCConfiguration::VCConfiguration()
  : UseOfMfc(UseMfc_StdWin),
    UseOfATL(UseATL_NotSet),
    WholeProgramOptimization(Optimization_None),
    ConfigurationType(Type_Unknown),
    CharacterSet(CharSet_NotSet) {
  for(size_t i = 0; i < ARRAY_COUNT(kToolNames); ++i) {
    ToolProperties.insert(make_pair(kToolNames[i], std::unordered_map<std::string, std::string>()));
  }
}

VCFile::VCFile():
ForcedInclude(false),
PrecompiledHeader(false),
CompileAsC(false){}