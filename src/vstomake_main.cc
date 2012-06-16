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

#include <sys/types.h>
#include <sys/stat.h>
#include <vector>
#include <unordered_map>

#include "make_file.h"
#include "libvs.h"
#include "output_option.h"
#include "utility.h"

using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::unordered_map;
using std::make_pair;


// @@ kDocumentation
static const char kDocumentation[] ="\
Makefile generation:\n\
  vstomake [input] [directory=./]\n\
Console output:\n\
  vstomake [input] [option] [configuration name]\n\
\n\
  [input]\n\
    A vcproj file\n\
\n\
  [directory]\n\
    Output directory. Current directory is the default.\n\
  Any file named \"Makefile\" within the specified directory\n\
  will be overwritten. Please omit trailing slashes.\n\
\n\
  [option]\n\
    Outputs properties of the given project file to stdout. An optional \n\
  prefix may be supplied in the form:\n\
    -option=prefix\n\
  where option is any of the following options and prefix is a user\n\
  defined string that may contain any of the macros listed in [macros]\n\
  If the -a option is not given then a valid configuration name is\n\
  expected. \n\n\
    Example:\n\
      vstomake vstomake.vcproj -D=\"$(CPPFLAGS) =\" \"Debug|Win32\"\n\
    Output:\n\
      $(CPPFLAGS) =-DWIN32 -D_DEBUG -D_CONSOLE -DVSTOMAKE_RUN_TESTS -DRAPIDXML_NO_EXCEPTIONS\n\
  \n\
    -a           output for all configurations\n\
    -i           forced include files \n\
    -I           external include directories\n\
    -L           external library directories used by the linker\n\
    -l           external libraries passed to the linker\n\
    -D           preprocessor defines in the specified configuration\n\
    -s           sources included in the specified configuration\n\
  \n\
  \n\
  [macros]\n\
     when using an option specifier of the form -option=prefix\n\
  The following strings found in prefix will be expanded:\n\n\
    $(Name)          project's name\n\
    $(Platform)      build platform name\n\
    $(Configuration) configuration's name\n\
    $(IntDir)        configuration's intermediate directory\n\
    $(OutDir)        configuration's output directory\n\
    $(Target)        configuration's target output name\n\
  \n";

int ErrorMessage(const string& msg) {
  puts(msg.c_str());
  puts(kDocumentation);
  return 1;
}

int main(int argc, char* argv[]) {
#ifdef VSTOMAKE_RUN_TESTS2
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
#else
  vector<string> args(argv, argv+argc);

  if(argc < 2) {
    return ErrorMessage("No input files.");
  }

  if(argv[1][0] == '-') {
    return ErrorMessage("No input project specified.");
  }

  string destination(".");
  bool output_option = false;
  if(argc > 2) {
    if(argv[2][0] == '-')
      output_option = true;
    else
      destination.assign(argv[2]);
  }
  
  // check input file
  struct stat info;
  int stat_result = stat(argv[1], &info);
  if(stat_result == 0 && !(info.st_mode & S_IFREG)){
    string err("Invalid input file: ");
    err.append(argv[1]);
    return ErrorMessage(err);
  }else if(stat_result == -1){
    string err("Non existant file: ");
    err.append(argv[1]);
    return ErrorMessage(err);
  }

  // check output destination
  memset(&info,0,sizeof (info));
  if(stat(destination.c_str(), &info) ==0) {
    if(info.st_mode & S_IFDIR) {
      destination.append("/Makefile");
    } else if(info.st_mode & S_IFREG) {
      destination.append(" is not a directory");
      return ErrorMessage(destination.c_str());
    }
  } else {
    destination.append(" is an invalid path.");
    return ErrorMessage(destination.c_str());
  }




  string errors;
  vs::Project project;
  vs::Project::Parse(argv[1], &project, &errors);

  if(!errors.empty()) {
    return ErrorMessage(errors);
  }


  if(output_option) {
    DoOutputOption(project, argc, argv);
  } else {
    Makefile makefile(project);

    // write the file
    destination.assign(AbsoluteFilePath(destination));
    ofstream outfile(destination.c_str());
    if(outfile.is_open()) {
      outfile << makefile.contents;
      outfile.close();
      printf("Output: %s\n", destination.c_str());
    } else {
      return ErrorMessage("Error writing Makefile");
    }
  }



  fflush(stdout);
  return 0;
#endif
}
