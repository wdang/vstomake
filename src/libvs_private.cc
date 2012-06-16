
#include "libvs_private.h"
#include "utility.h"

#include <algorithm>
#include <fstream>
#include <iterator>
#include <utility>

#include <rapidxml/rapidxml.hpp>

using std::back_inserter;
using std::copy;
using std::copy_if;
using std::ifstream;
using std::make_pair;
using std::move;
using std::remove_if;
using std::string;
using std::unordered_map;
using std::vector;

typedef rapidxml::xml_document<>           XMLDocument;
typedef rapidxml::xml_node<>               XMLNode;
typedef rapidxml::xml_attribute<>          XMLAttribute;
typedef std::unordered_map<string, string> MacroMap;

namespace vs {

using namespace vs::Enum;


Format GetFormat(const std::string& path) {
  static const char* kSolution2008 = {"Microsoft Visual Studio Solution File, Format Version 10.00"};
  static const char* kSolution2010 = {"Microsoft Visual Studio Solution File, Format Version 11.00"};

  Format format = Format_unknown;

  string buffer("");
  FileToString(path, &buffer);

  bool solution_file = buffer.find(kSolution2008) != string::npos;

  if(!solution_file) {
    solution_file = buffer.find(kSolution2010) != string::npos;
  }


  if(solution_file) {
    format = Format_sln;
  } else {
    XMLDocument doc;
    doc.parse<0>(&buffer[0]);

    if(doc.first_node("VisualStudioProject")) {
      format = Format_vcproj;
    } else if(XMLNode* project = doc.first_node("Project")) {
      XMLNode* import = project->first_node("Import");

      while(import) {
        if(XMLAttribute* attr = import->first_attribute("Project")) {

          if(strstr(attr->value(), "Microsoft.Cpp") != NULL) {
            format = Format_vcxproj;
            break;
          } else if(strstr(attr->value(), "Microsoft.CSharp") != NULL) {
            format = Format_csproj;
            break;
          }
        }

        import = import->next_sibling();
      }
    }
  }
  return format;
}

}