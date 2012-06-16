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
#include "internal.h"
#include "make_file.h"
#include "libvs.h"
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


static string GenerateCXXFlags(const vs::VCCLCompilerTool* cl) {
  stringstream ss("");
  if(strcmp(cl->ForcedIncludeFiles(), "") != 0) {
    ss << " -include " << Replace(ToUnixPaths(cl->ForcedIncludeFiles()), ";", " -include ");
  }

  if(strcmp(cl->AdditionalIncludeDirectories(), "") != 0) {
    ss << " -I" << Replace(ToUnixPaths(cl->AdditionalIncludeDirectories()), ";", " -I");
  }

  return ss.str();
}

static string GenerateCPPFlags(const vs::VCCLCompilerTool* cl) {
  stringstream ss;
  ss << "-D" << Replace(cl->PreprocessorDefinitions(), ";", " -D");
  return ss.str();
}

struct MakefileGen {
  MakefileGen() {}

  MakefileGen(const vs::Project& project);
  void   Initialize(const vs::Configuration& config);
  string GlobalVariables();
  string TopLevelBuildRules();
  string BuildPrologue();
  string ObjectBuildRules();
  string PreBuildRule();
  string BuildRule();
  string PostBuildRule();
  string BuildEpilogue();

  vector<vs::File*> build_sources;

  set<string>       general_rules; //holds Debug, Release etc
  set<string>       specific_rules; //holds DebugWin32, ReleaseWin32 etc

  stringstream      ss;


  string rule;
  string name;
  string outdir, intdir;
  string config_objs, config_srcs;
  string precompiled;
  string project_name;
  string project_dir;
  string config_name;
  string config_platform;


  const vector<vs::File*>*    files;
  const vs::VCCLCompilerTool* cl;

  vs::Enum::Type type;
};

// TODO(wdang): look into serialization implementations
// TODO(wdang): provide rules for static libraries
// TODO(wdang): provide rules for shared libraries
Makefile::Makefile(const vs::Project& project) {
  std::stringstream ss("");

  MakefileGen gen(project);

  ss << "\n#### Build variables ####\n"
     << gen.GlobalVariables() << "\n";

  ss << "\n#### Build rules ####\n"
     << gen.TopLevelBuildRules();

  foreach(auto& config, project.Configurations) {
    gen.Initialize(config);
    ss << gen.BuildPrologue() << "\n"
       << gen.ObjectBuildRules() << "\n"
       << gen.PreBuildRule() << "\n"
       << gen.BuildRule() << "\n"
       << gen.PostBuildRule() << "\n"
       << gen.BuildEpilogue() << "\n\n";
  }
  this->contents = ss.str();
}

MakefileGen::MakefileGen(const vs::Project& project)
  : project_name(project.Name),
    project_dir(ToUnixPaths(project.Path)) {
  size_t sep = project.Path.find_last_of("/\\");
  if(sep == string::npos) {
    project_dir.assign(".");
  } else {
    project_dir.assign(project.Path.substr(0, sep));
  }

  foreach(auto& rule, project.Configurations) {
    general_rules.insert(rule.ConfigurationName);
    specific_rules.insert(rule.ConfigurationName + rule.Platform);
  }
}

void MakefileGen::Initialize(const vs::Configuration& config) {
  intdir.assign(ToUnixPaths(config.IntermediateDirectory));
  outdir.assign(ToUnixPaths(config.OutputDirectory));
  rule.assign(config.ConfigurationName + config.Platform);
  name.assign(config.Name);
  config_objs.assign(rule + "_OBJS");
  config_srcs.assign(rule + "_SRCS");
  files = &config.Files;
  cl    = &config.CLCompilerTool;
  type  = config.ConfigurationType;
  precompiled.assign("");

  foreach(auto& f, config.Files) {
    if(f->Precompiled) {
      precompiled.assign(ToUnixPaths(f->RelativePath));
    }
  }
}

std::string MakefileGen::GlobalVariables() {
  ss.str("");
  ss << "CFLAGS += $(CURRENT_CFLAGS)\n"
     << "CPPFLAGS += $(CURRENT_CPPFLAGS)\n"
     << "CXXFLAGS += --std=gnu++0x $(CURRENT_CXXFLAGS) -I" << project_dir <<" -Wall -Wextra\n"
     << "LDFLAGS  += $(CURRENT_LDFLAGS)\n"
     << "ARFLAGS   = -rcs $(CURRENT_ARFLAGS)\n"
     << "PROJECT_NAME      := " <<project_name << "\n"
     << "OUTDIR             = $(CURRENT_OUTDIR)\n"
     << "INTDIR             = $(CURRENT_INTDIR)\n"
     << "TARGET             = $(CURRENT_OUTDIR)/$(CURRENT_TARGET)\n";
  return ss.str();
}

std::string MakefileGen::TopLevelBuildRules() {
  ss.str("");

  ss << "default:\n"
     << "\t@echo Available build rules:\n";
  foreach(auto& rule, general_rules) {
    ss << "\t@echo "  << rule << "\n";
  }
  foreach(auto& rule, specific_rules) {
    ss << "\t@echo "  << rule << "\n";
  }

  ss << "\t@echo all\n"
     << "\t@echo clean\n"
     << "\n.PHONY: all clean\n\nall:";

  foreach(auto& rule, general_rules) {
    ss <<" "<< rule;
  }

  // clean rules
  ss <<"\n\nclean: \n"
     <<"\t@rm -vf ";

  foreach(auto& name, specific_rules) {
    ss <<"$("<< name << "_OBJS) ";
  }
  ss <<"\n\n";

  foreach(auto& rule, general_rules) {
    ss << rule << ":";
    foreach(auto& name, specific_rules) {
      if(name.find(rule) != string::npos) {
        ss <<" "<< name;
      }
    }
    ss << "\n\n";
  }

  return ss.str();
}

std::string MakefileGen::BuildPrologue() {
  ss.clear();
  ss.str("");
  ss << "#### Configuration: " << config_name<<" ####\n"
     << "####      Platform: " << config_platform<<" ####\n"
     << rule <<"_SRCS :=";

  // source listing
  vector<vs::File*> sources;
  FilterCPPSources(*files, &sources);

  foreach(auto& src, sources) {
    ss << "\\\n"<< ToUnixPaths(StripCurrentDirReference(src->RelativePath));
  }

  // prebuild stats

  ss << "\n" << rule << "Stats:\n"
     << "\t@echo \"Configuration: " << name <<"\"\n"
     << "\t@echo \"CPPFLAGS:     \" $(CPPFLAGS)\n"
     "\t@echo \"CFLAGS:       \" $(CFLAGS)\n"
     "\t@echo \"CXXFLAGS:     \" $(CXXFLAGS)\n"
     "\t@echo \"ARFLAGS:      \" $(ARFLAGS)\n"
     "\t@echo \"LDFLAGS:      \" $(LDFLAGS)\n"
     "\t@echo \"Intermediates:\" $(INTDIR)\n"
     "\t@echo \"Outputs:      \" $(OUTDIR)\n"
     "\t@echo \"Target:       \" $(TARGET)\n\n";

  // source to .o pathsubst
  string objects_o(intdir + "/%.o");

  objects_o.append("/%.o");
  ss << this->config_objs << " := $(patsubst %.cc," << this->intdir << "/%.o,$(notdir $("<< this->config_srcs <<")))\n"
     << this->config_objs << " := $(patsubst %.cpp,"<< this->intdir << "/%.o,$(" << this->config_objs <<"))\n"
     << this->config_objs << " := $(patsubst %.cxx,"<< this->intdir << "/%.o,$(" << this->config_objs <<"))\n\n";


  return ss.str();
}

std::string MakefileGen::ObjectBuildRules() {
  ss.clear();
  ss.str("");

  std::set<string> src_paths;
  std::stringstream forced_include("");
  vector<vs::File*> sources;
  FilterCPPSources(*files, &sources);

  foreach(auto src, sources) {
    string path(ToUnixPaths(StripCurrentDirReference(src->RelativePath)));
    string path_part(path.substr(0, path.find_last_of('/')+1));
    string ext(path.substr(path.find_last_of('.')));

    string src_wildcard("");
    src_wildcard.reserve(path.size());
    src_wildcard.append(path_part);
    src_wildcard.append("%");
    src_wildcard.append(ext);
    src_paths.insert(src_wildcard);
  }

  std::string recipe(intdir + "/%.o : ");
  foreach(auto& src_wildcard, src_paths) {
    ss << recipe << src_wildcard <<"\n"
       << "\t@echo $<\n"
       << "\t@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $<"<< forced_include.str() <<" -o $(INTDIR)/$(notdir $@)\n";
  }


  return ss.str();
}

std::string MakefileGen::PreBuildRule() {
  ss.clear();
  ss.str("");

  // prebuild rule
  ss << rule << "Prebuild"<< ":\n"
     << "\t@mkdir -p " <<  intdir << "\n"
     << "\t@mkdir -p " << outdir << "\n\n";

  if(!precompiled.empty()) {
    std::string pch(precompiled.substr(0, precompiled.find_last_of(".")));
    pch.append(".h.gch");
    ss << pch << ":\n"
       << "\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c " << precompiled  << " -o " << pch << "\n";
  }

  return ss.str();
}

std::string MakefileGen::BuildRule() {
  ss.clear();
  ss.str("");
  // Main build rule
  string build_rule(rule+"Build");
  ss << build_rule << ":CURRENT_OUTDIR    :=" << outdir<< "\n"
     << build_rule << ":CURRENT_INTDIR    :=" << intdir << "\n"
     << build_rule << ":CURRENT_CXXFLAGS  :=" << GenerateCXXFlags(cl) << "\n"
     << build_rule << ":CURRENT_CPPFLAGS  :=" << GenerateCPPFlags(cl) << "\n";

  string prerequisites(rule + "Stats " + "$(" + config_objs +")");
  switch(type) {
    case vs::Enum::Type_Application:
      ss << build_rule << ":CURRENT_TARGET    :=" << "$(PROJECT_NAME)\n"
         << build_rule << ":" << prerequisites << "\n"
         << "\t$(CXX)  $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) "<<" $(" << config_objs << ")" << " -o $(TARGET)\n\n";
      break;

    case vs::Enum::Type_DynamicLibrary:
      ss << build_rule << ":CURRENT_TARGET        :=" << "lib$(PROJECT_NAME).so\n"
         << build_rule << ":" << prerequisites << "\n"
         << "\t$(CXX) -shared $(CPPFLAGS) $(CXXFLAGS) "<<" $(" << config_objs << ")" <<" -o $(OutDir)/lib$(PROJECT_NAME).so\n\n";
      break;

    case vs::Enum::Type_StaticLibrary:
      ss << build_rule << ":CURRENT_TARGET       :=" << "lib$(PROJECT_NAME).a\n"
         << build_rule << ":" << prerequisites << "\n"
         << "\t$(AR) $(ARFLAGS) $(TARGET)" <<" $(" << config_objs << ")\n\n";
      break;
   
    case vs::Enum::Type_Unknown:
    case vs::Enum::Type_Utility:
    default:
      break;
  }
  return ss.str();
}

std::string MakefileGen::PostBuildRule() {
  ss.clear();
  ss.str("");
  ss << rule << ": " << rule << "Prebuild " << rule << "Build ";
  return ss.str();
}

std::string MakefileGen::BuildEpilogue() {
  ss.clear();
  ss.str("");
  ss << "clean." << rule << ":\n\t"
     <<"@rm -vf $("<<config_objs << ")";
  return ss.str();
}
