#include "precompiled.h"
#include "output_option.h"
#include "internal.h"
#include "utility.h"

using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::unordered_map;
using std::make_pair;

enum OutputOption {
  OutputOption_Invalid       = 0,
  OutputOption_Configs       = 1,
  OutputOption_ForcedIncludes= 2,
  OutputOption_IncludeDirs   = 4,
  OutputOption_LibraryDirs   = 8,
  OutputOption_Libraries     = 16,
  OutputOption_Defines       = 32,
  OutputOption_Sources       = 64
};

bool ParseCommandLine(const vector<string>& args, unordered_map<string, string>* options,
                      const char* delimiter = "=") {
  if(args.size() < 2)
    return false;
  for(int i = 0, end = args.size(); i < end; ++i) {
    string option(args[i]);
    size_t index = option.find(delimiter, 0);

    if(index != string::npos) {
      string key   = option.substr(0, index);
      string value = option.substr(index+1);
      options->insert(make_pair(key, value));
    } else {
      options->insert(make_pair(option, ""));
    }
  }
  return options->size() > 0;
}

unordered_map<string, string> Macros;




void OutputForConfiguration(const vs::Configuration& config, OutputOption flags, unordered_map<string, string>& options) {
  std::ostringstream ss;
  if(flags & OutputOption_ForcedIncludes) {
    ss << options["-include"];
    ss << "Not implemented";    
    ss << '\n';
  }

  if(flags & OutputOption_IncludeDirs) {
    ss << options["-I"];
    std::string include_dirs("-I");
    include_dirs.append(config.CLCompilerTool.AdditionalIncludeDirectories());
    Replace(&include_dirs, ";", " -I");
    ss <<include_dirs << '\n';
  }

  if(flags & OutputOption_Libraries) {
    ss << options["-l"];
    ss << "Not implemented";
    ss << '\n';
  }

  if(flags & OutputOption_LibraryDirs) {
    ss << options["-L"];
    ss << "Not implemented";
    ss << '\n';
  }

  if(flags & OutputOption_Defines) {
    ss << options["-D"];
    std::string defines("-D");
    defines.append(config.CLCompilerTool.PreprocessorDefinitions());
    Replace(&defines, ";", " -D");
    ss <<defines << '\n';
  }

  if(flags & OutputOption_Sources) {
    ss << options["-s"];
    vector<vs::File*> sources;
    sources.reserve(config.Files.size());
    vs::FilterCPPSources(config.Files, &sources);
    for(size_t i = 0, end = sources.size(); i < end; ++i) {
      const std::string& src = ToUnixPaths(sources[i]->RelativePath);
      if(i+1 != end) {
        ss<<src << "\\\n";
      } else {
        ss<<src;
      }
    }
  }

  printf(ss.str().c_str());
}


void DoOutputOption(const vs::Project& project, int argc, char** argv) {
  vector<std::string> args(argv,argv+argc);
  
  const string& config_name           = args.back();
  const vs::Configuration* config_ptr = 0;
  foreach(const vs::Configuration& c, project.Configurations) {
    if(config_name.compare(c.Name) == 0) {
      config_ptr = &c;
      break;
    }
  }

  unordered_map<string, string> options;
  ParseCommandLine(args, &options);
  bool output_all = options.find("-a") != options.end();

  int flags       = OutputOption_Invalid;
  for(size_t i = 2, end = args.size(); i < end; ++i) {
    switch(args[i][1]) {
      case 'c': flags |=  OutputOption_Configs; break;
      case 'i': flags |=  OutputOption_ForcedIncludes; break;
      case 'I': flags |=  OutputOption_IncludeDirs; break;
      case 'L': flags |=  OutputOption_LibraryDirs; break;
      case 'l': flags |=  OutputOption_Libraries; break;
      case 'D': flags |=  OutputOption_Defines; break;
      case 's': flags |=  OutputOption_Sources; break;
    }
  }

  if(!flags) {
    printf("Invalid option specified\n");
    return;
  }


  std::ostringstream ss;

  if(!output_all && !config_ptr && !(flags & OutputOption_Configs)) {
    printf("%s is not a valid configuration name.\n", config_name.c_str());
    return;
  }
  if(output_all) {

    foreach(const vs::Configuration& c, project.Configurations) {
      unordered_map<string, string> expanded = options;
      foreach(auto& m, expanded) {
        string prefix(m.second);
        if(!prefix.empty()) {
          Replace(&prefix, "$(Name)", project.Name);
          Replace(&prefix, "$(Platform)", c.Platform);
          Replace(&prefix, "$(Configuration)", c.ConfigurationName);
          Replace(&prefix, "$(IntDir)", c.IntermediateDirectory);
          Replace(&prefix, "$(OutDir)", c.OutputDirectory);
        }
        expanded[m.first] = prefix;
      }
      OutputForConfiguration(c, (OutputOption)flags, expanded);
    }
  } else {
    foreach(auto& o, options) {
      string replacement(o.second);
      Replace(&replacement, "$(Name)", project.Name);
      Replace(&replacement, "$(Platform)", config_ptr->Platform);
      Replace(&replacement, "$(Configuration)", config_ptr->ConfigurationName);
      Replace(&replacement, "$(IntDir)", config_ptr->IntermediateDirectory);
      Replace(&replacement, "$(OutDir)", config_ptr->OutputDirectory);
      options[o.first] = replacement;
    }

    OutputForConfiguration(*config_ptr, (OutputOption)flags, options);
  }
}
