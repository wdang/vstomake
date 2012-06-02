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
#include "makefile.h"
#include "vcproject.h"
#include "vclinkertool.h"
#include "utility.h"
#include "vcclcompilertool.h"
#include <set>
using std::ifstream;
using std::istringstream;
using std::make_pair;
using std::set;
using std::string;
using std::stringstream;
using std::unordered_map;
using std::vector;
using std::copy_if;

string GenerateCXXFlags(const string&, const string&,const VCCLCompilerTool& cl) {
  stringstream ss("");

  if (strcmp(cl.AdditionalIncludeDirectories(), "") != 0) {
    ss << "-I" << Replace(ToUnixPaths(cl.AdditionalIncludeDirectories()), ";", " -I");
  }

  return ss.str();
}

string GenerateCPPFlags(const std::string&, const std::string&, const VCCLCompilerTool& cl) {
  stringstream ss;
  ss << "-D" << Replace(cl.PreprocessorDefinitions(), ";", " -D");
  return ss.str();
}

string GenerateARFlags(const VCProject::Configuration& config, const VCCLCompilerTool& cl);
string GenerateLDFlags(const VCProject::Configuration& config, const VCLinkerTool& link);

//  The top level build rules are separated by configuration then by platform.
//  For a project with a Debug and Release configuration that
//  is targeting "Win32", GenerateTopLevelBuildRules will return:
//
//  all:  Debug Release
//  Debug: DebugWin32
//  Release: ReleaseWin32
string GenerateTopLevelBuildRules(const vector<VCProject::Configuration>& configurations) {
  stringstream ss;
  set<std::string> general_rules; //holds Debug, Release etc
  set<std::string> specific_rules; //holds DebugWin32, ReleaseWin32 etc

  foreach(auto& rule, configurations) {
    general_rules.insert(rule.name);
    specific_rules.insert(rule.name + rule.platform);
  }

  ss << "default:\n"
     << "\t@echo Available build rules:\n";
  foreach(auto& rule, general_rules) {
    ss << "\t@echo "  << rule << "\n";
  }
  foreach(auto& rule, specific_rules) {
    ss << "\t@echo "  << rule << "\n";
  }
  ss << "\t@echo all\n"
     << "\t@echo clean\n";

  ss << "all: ";

  foreach(auto& rule, general_rules) {
    ss <<" "<< rule;
  }
  ss <<"\n";
  ss <<"clean:\n"
     <<"\t@rm -vf ";
  foreach(auto& name, specific_rules) {
    ss <<"$("<< name << "Objects) ";
  }
  ss <<"\n";

  foreach(auto& rule, general_rules) {
    ss << rule << ":";
    foreach(auto& name, specific_rules) {
      if (name.find(rule) != string::npos) {
        ss <<" "<< name;
      }
    }
    ss << "\n";
  }
  ss <<"\n";

  return ss.str();
}


// Build rules are separated by configuration and platform.
// The generated Makefile variables will have the following convention:
// ConfigurationPlatformVariable
// where 'Variable' is one of
//
// -Prebuild
// -Precompile
// -Postbuild
// -Objects
// -Sources
//
// The build rule that starts the build will be named:
// ConfigurationPlatform
// TODO(wdang): provide rules for c sources
std::string GenerateBuildRule(const VCProject::Configuration& config) {
  std::stringstream ss;
  VCCLCompilerTool cl(config);



  auto iter = config.properties.find("VCCLCompilerTool");
  if (iter != config.properties.end()) {
    const auto& properties = iter->second;


    auto pch_iter  = properties.find("PrecompiledHeaderThrough");

    bool using_pch = pch_iter != properties.end();
    string pch(ToUnixPaths(cl.PrecompiledHeaderThrough()));

    string pch_include("");
    string gch("");
    if (using_pch) {
      gch.assign(pch);
      gch.append(".gch");
      pch_include.assign("-include ");
      pch_include.append(pch);
    }


    // configuration specific variables
    string rule_prefix(config.name + config.platform);
    string build(rule_prefix);
    build.append("Build");

    string sources(rule_prefix);
    sources.append("Sources");

    string objects(rule_prefix);
    objects.append("Objects");
    
    string intdir(ToUnixPaths(config.intermediate_dir));
    string outdir(ToUnixPaths(config.output_dir));
    
    
    string objects_o(ToUnixPaths(config.intermediate_dir));
    objects_o.append("/%.o");
    // source to .o pathsubst
    ss << objects << " := $(patsubst %.cc," << intdir << "/%.o,$(notdir $("<< sources <<")))\n"
       << objects << " := $(patsubst %.cpp,"<< intdir << "/%.o,$(" << objects <<"))\n"
       << objects << " := $(patsubst %.cxx,"<< intdir << "/%.o,$(" << objects <<"))\n\n";

    if (using_pch) {
      ss << gch << ": " << pch << "\n"
         << "\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c " << pch << "\n\n";
    }


    // build rules
    std::string prebuild_rule(rule_prefix);
    prebuild_rule.append("Prebuild");

    string build_rule(rule_prefix);
    build_rule.append("Build:");

    string main_build_rule(rule_prefix);
    main_build_rule.append(":");

    // prebuild rule
    ss << prebuild_rule << ":\n"
       << "\t@mkdir -p " << outdir << "\n"
       << "\t@mkdir -p " << intdir << "\n\n";   

    // Main build rule
    ss << build_rule << "CurrentOutDir    :=" << outdir << "\n"
       << build_rule << "CurrentIntDir    :=" << intdir << "\n"
       << build_rule << "CURRENT_CXXFLAGS :=" << GenerateCXXFlags(config.name,config.platform,cl) << "\n"
       << build_rule << "CURRENT_CPPFLAGS :=" << GenerateCPPFlags(config.name,config.platform,cl) << "\n"       
       << build_rule << gch <<" $(" << objects << ")\n";
     
    
    switch (config.type) {
    case VCProject::Type_Application:
      ss << "\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) "<<" $(" << objects << ")" << " -o $(TargetDir)\n\n";
      break;
    case VCProject::Type_DynamicLibrary:
      ss << "\t$(CXX) -shared $(CPPFLAGS) $(CXXFLAGS) "<<" $(" << objects << ")" <<" -o $(OutDir)/lib$(ProjectName).so\n\n";
      break;
    case VCProject::Type_StaticLibrary:
      ss << "\t$(AR) $(ARFLAGS) $(OutDir)/lib$(ProjectName).a " <<" $(" << objects << ")\n\n";
      break;
    }


    // todo post-build commands
    ss << main_build_rule << prebuild_rule << " " << build << "\n\n";
  }
  return ss.str();
}

// TODO(wdang): look into serialization implementations
// TODO(wdang): provide rules for static libraries
// TODO(wdang): provide rules for shared libraries
Makefile::Makefile(const VCProject& project) {


  std::stringstream ss("Makefile.mk");


  // TODOD(wdang): organize predefined flags
  std::stringstream cflags("");
  std::stringstream cppflags("");
  std::stringstream cxxflags("");
  std::stringstream ldflags("");
  std::stringstream arflags("");



  cflags << "$(CURRENT_CFLAGS)";

  cppflags << "$(CURRENT_CPPFLAGS)";


  size_t found = project.path.find_last_of("/\\");
  std::string project_dir(".");
  if (found != string::npos) {
    project_dir.assign(project.path.substr(0, found));
  }
  
  cxxflags << "--std=gnu++0x"
           << " -I" << ToUnixPaths(project_dir)
           << " $(CURRENT_CXXFLAGS)";

  ldflags << "$(CURRENT_LDFLAGS)";

  arflags << "-rcs "
          << "$(CURRENT_ARFLAGS)";



  ss << "#### This Makefile has been machine generated from vstomake 0.1.0 ####\n\n"
     << "#### Builtin variables ####\n"
     << "CFLAGS   += "<< cflags.str()   << "\n"
     << "CPPFLAGS += "<< cppflags.str() << "\n"
     << "CXXFLAGS += "<< cxxflags.str() << "\n"
     << "LDFLAGS  += "<< ldflags.str()  << "\n"
     << "ARFLAGS   = "<< arflags.str()  << "\n"

     << "\n#### Project variables ####\n"
     << "ProjectName       := "<<project.name<<"\n"
     << "TargetDir          = $(OutDir)/$(ProjectName)\n"
     << "OutDir             = $(CurrentOutDir)\n"
     << "IntDir             = $(CurrentIntDir)\n";

  ss << "\n#### Build rules ####\n"
     << GenerateTopLevelBuildRules(project.configurations);



  foreach(auto& config, project.configurations) {
    // Exclude non c++ source files
    static const char* kCPPExtensions[] = {".cc", ".cpp", ".cxx", ".c++", ".C", ".cp", ".CPP"};

    vector<VCProject::File> sources;
    sources.reserve(config.files.size());

    copy_if(config.files.begin(), config.files.end(), std::back_inserter(sources),
        [] (const VCProject::File& file)->bool {
          foreach(auto* ext, kCPPExtensions) {
            if (file.path.find(ext) != string::npos) {
              return true;
            }
          }
          return false;
        });


    ss << "#### Configuration: " << config.name     <<" ####\n"
       << "####      Platform: " << config.platform <<" ####\n"
       << config.name << config.platform <<"Sources :=";
    
    set<string> src_paths;
    
    foreach(auto& src, sources) {
      string path(ToUnixPaths(StripCurrentDirReference(src.path)));
      string path_part(path.substr(0,path.find_last_of('/')+1));
      string ext(path.substr(path.find_last_of('.')));
      
      string src_wildcard("");
      src_wildcard.reserve(path.size());
      src_wildcard.append(path_part);
      src_wildcard.append("%");
      src_wildcard.append(ext);
      src_paths.insert(src_wildcard);
      
      ss << "\\\n"<< ToUnixPaths(StripCurrentDirReference(src.path));
    }
   ss << "\n\n";
   foreach(auto& src_wildcard, src_paths){
     ss << ToUnixPaths(config.intermediate_dir) << "/%.o : "<< src_wildcard <<"\n"
     << "\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $(IntDir)/$(notdir $@)\n";
   }
   ss << "\n\n" << GenerateBuildRule(config) << "\n";
  }

  ss << std::endl;
  this->contents = ss.str();
}
