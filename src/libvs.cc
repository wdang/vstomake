#include "libvs_private.h"

#include "utility.h"
#include "vcproject_parser.h"
#include "msbuild_parser.h"

#include <unordered_map>

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <rapidxml/rapidxml_utils.hpp>

#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <algorithm>

#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH
#include "string_tokenizer.h"

// Represents a VCProject file
// VCProject parses a .vcproj on construction
typedef rapidxml::xml_document<>  XMLDocument;
typedef rapidxml::xml_node<>      XMLNode;
typedef rapidxml::xml_attribute<> XMLAttribute;

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
using std::copy_if;

// error handler for rapidxml (when exceptions are disabled)
void rapidxml::parse_error_handler(const char*, void*) {
  exit(1);
}


void ParseVCXPROJ();
void ParseVCPROJ();
void ParseCSPROJ();
void ParseSLN();




namespace vs {


bool Project::Parse(const std::string& filepath, Project* ptr, std::string* errors /*= 0*/) {
  using namespace Enum;
  std::string dummy("");
  if(!errors) errors = &dummy;

  Enum::Format format = GetFormat(filepath);
  if(format == Enum::Format_unknown) {
    errors->append("Unknown file format: ");
    errors->append(filepath);
    return false;
  }

  ptr->Path.assign(filepath);
  ptr->AbsolutePath.assign(AbsoluteFilePath(filepath));
  size_t separator = ptr->AbsolutePath.find_last_of("\\/");
  ptr->ProjectFile      = ptr->AbsolutePath.substr(separator+1);
  ptr->ProjectDirectory = ptr->AbsolutePath.substr(0, separator);
  
  ProjectParser* parser = 0;
  VCProjectParser vcproj;
  MSBuildParser msbuild;

  switch(format) {
    case Format_sln:
      errors->append("Solution files aren't supported yet.");
      break;

    case Format_vcproj:
      parser = &vcproj;
      break;

    case Format_vcxproj:
      errors->append("vc10 aren't supported yet.");
      return false;

    case Format_csproj:
      errors->append("csharp projects aren't supported yet.");
      return false;
      
    case Format_unknown:
    default:
      errors->append("Format unknown.");    
      return false;
  }


  string buffer("");
  FileToString(filepath, &buffer);
  buffer.push_back('\0');
   
   
  if(parser->Parse(filepath)) {
    unordered_map<string, string> props;

    if(!parser->ProjectProperties(&props)) {
      errors->append(filepath);
      errors->append(" does not contains any global properties.");
      return false;
    }

    ptr->Name.assign(props["Name"]);
    ptr->ProjectType.assign(props["ProjectType"]);
    ptr->RootNamespace.assign(props["RootNamespace"]);
    ptr->ProjectGUID.assign(props["ProjectGUID"]);
    ptr->Keyword.assign(props["Keyword"]);
    ptr->Version.assign(props["Version"]);
    

    if(!parser->Configurations(&ptr->Configurations)) {
      errors->append(filepath);
      errors->append(" does not contains any configurations.");
      return false;
    }


    if(!parser->Files(&ptr->Files)) {
      errors->append(filepath);
      errors->append(" does not contains any files.\n");
      // continue here
    }


    foreach(auto& c, ptr->Configurations) {

      // assign tool pointers
      auto& cltool = c.ToolProperties["VCCLCompilerTool"];
      c.CLCompilerTool.properties = &cltool;

      string forced_includes(cltool["ForcedIncludeFiles"]);

      foreach(auto& file, ptr->Files) {
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

  return false;
}
}
