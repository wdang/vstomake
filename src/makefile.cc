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

//  The top level build rules are separated by configuration then by platform.
//  For a project with a Debug and Release configuration that
//  is targeting "Win32", GenerateTopLevelBuildRules will return:
//
//  all:  Debug Release
//  Debug: DebugWin32
//  Release: ReleaseWin32
string GenerateTopLevelBuildRules(const vector<VCProject::Configuration>& configurations) {
  stringstream ss;
  set<std::string> configs; //holds Debug, Release etc
  set<std::string> config_names; //holds DebugWin32, ReleaseWin32 etc

  foreach(auto& config, configurations) {
    configs.insert(config.name);
    config_names.insert(config.name + config.platform);
  }

  ss << "default:\n"
     << "\t@echo Available build rules:\n";
  foreach(auto& config, config_names) {
    ss << "\t@echo "  << config << "\n";
  }
  ss << "\t@echo all\n"
     << "\t@echo clean\n";

  ss << "all: ";

  foreach(auto& config, configs) {
    ss <<" "<< config;
  }
  ss <<"\n";

  ss <<"clean:\n"
     <<"\trm -vf ";
  foreach(auto& name, config_names) {
    ss <<"$("<< name << "Objects) ";
  }
  ss <<"\n";

  foreach(auto& config, configs) {
    ss << config << ":";
    foreach(auto& name, config_names) {
      if (name.find(config) != string::npos) {
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
  VCCLCompilerTool cl(config.properties.find("VCCLCompilerTool")->second);


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
    string variable(config.name + config.platform);
    string build(variable);
    build.append("Build");

    std::string sources(variable);
    sources.append("Sources");

    std::string objects(variable);
    objects.append("Objects");

    std::string objects_o(ToUnixPaths(config.intermediate_dir));
    objects_o.append("/%.o");
    // source to .o pathsubst
    ss << objects << " := $(patsubst %.cc, " << objects_o << ",$("<< sources <<"))\n"
       << objects << " := $(patsubst %.cpp," << objects_o << ",$("<< objects <<"))\n"
       << objects << " := $(patsubst %.cxx," << objects_o << ",$("<< objects <<"))\n\n";

    if (using_pch) {
      ss << gch << ": " << pch << "\n"
         << "\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c " << pch << "\n\n";
    }


    // build rules
    std::string prebuild_rule(variable);
    prebuild_rule.append("Prebuild");

    string build_rule(variable);
    build_rule.append("Build:");

    string main_build_rule(variable);
    main_build_rule.append(":");
    // TODO(wdang): prebuild build rule
    ss << prebuild_rule << ":\n\n";


    // .o build rule, accommodate for all c++ extensions
    ss << objects_o << ": %.cc\n"
       << "\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< "<< pch_include <<" -o $@\n"
       << objects_o << ": %.cpp\n"
       << "\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< "<< pch_include <<" -o $@\n"
       << objects_o << ": %.cxx\n"
       << "\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< "<< pch_include <<" -o $@\n\n";

    // Main build rule
    ss << build_rule << "OutDir           :=" << ToUnixPaths(config.output_dir) << "\n"
       << build_rule << "IntDir           :=" << ToUnixPaths(config.intermediate_dir) << "\n"
       << build_rule << "CURRENT_CXXFLAGS := -I. -I" << Replace(ToUnixPaths(cl.AdditionalIncludeDirectories()), ";", " -I") << "\n"
       << build_rule << "CURRENT_CPPFLAGS := -D" << Replace(cl.PreprocessorDefinitions(), ";", " -D") << "\n"
       << build_rule << gch <<" $(" << objects << ")\n"
       << "\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(" << objects << ") -o $(TargetDir)\n\n";

    //
    ss << main_build_rule << prebuild_rule << " " << build << "\n\n";



    // todo post-build commands
  }
  return ss.str();
}

// TODO(wdang): look into serialization implementations
// TODO(wdang): provide rules for static libraries
// TODO(wdang): provide rules for shared libraries
Makefile::Makefile(const VCProject& project) {


  std::stringstream ss("Makefile.mk");

  ss << "#### This Makefile has been machine generated from vstomake 0.1.0 ####\n\n"
     << "#### Builtin variables ####\n"
     << "CFLAGS   += $(CURRENT_CFLAGS)\n"
     << "CPPFLAGS += $(CURRENT_CPPFLAGS)\n"
     << "CXXFLAGS += $(CURRENT_CXXFLAGS) -std=gnu++0x\n"
     << "LDFLAGS  +=\n"
     << "ARFLAGS  +=\n\n"

     << "#### Project variables ####\n"
     << "ProjectName       := "<<project.name<<"\n"
     << "TargetDir          = $(OutDir)/$(ProjectName)\n\n";

  ss << "#### Build rules ####\n"
     << GenerateTopLevelBuildRules(project.configurations);

  foreach(auto& config, project.configurations) {
    // Exclude non c++ source files
    static const char* kCPPExtensions[] = {".cc",".cpp",".cxx",".c++",".C",".cp",".CPP"};  
      
    vector<VCProject::File> sources;
    sources.reserve(config.files.size());
       
    copy_if(config.files.begin(),config.files.end(),std::back_inserter(sources),
    [](const VCProject::File& file)->bool{
      foreach(auto* ext, kCPPExtensions){
        if(file.path.find(ext) != string::npos){
          return true;
        }
      }
      return false;
    });
    
    
    ss << "#### Configuration: " << config.name     <<" ####\n"
       << "####      Platform: " << config.platform <<" ####\n"
       << config.name << config.platform <<"Sources :=";
    foreach(auto& src, sources) {
     ss << "\\\n"<< ToUnixPaths(StripCurrentDirReference(src.path));
    }
    ss << "\n\n" << GenerateBuildRule(config) << "\n";
  }

  ss << std::endl;
  this->contents = ss.str();
}
