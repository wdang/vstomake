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
#include "msvc_object_model.h"
#include "make_file.h"
#include <io.h>
#include <windows.h>
#include <sys/stat.h>
#include <conio.h>

// @@ kDocumentation
static const char kDocumentation[] =
  "vstomake: Convert Visual Studio project file(s) to a GNU Makefile.\n\
Usage:\n\
  vstomake [input] [directory=./]\n\
\n\
  [input]\n\
    *.vcproj\n\
\n\
  [directory]\n\
      Output directory. Current directory is the default.\n\
    Any file named \"Makefile\" within the specified directory\n\
    will be overwritten. Please omit trailing slashes.";

int ErrorMessage(const std::string& msg) {
  puts(msg.c_str());
  puts(kDocumentation);
  return 1;
}

int main(int argc, char* argv[]) {  
#if 0
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
#else 
  using std::string;
  using std::ifstream;
  using std::ofstream;
  
  //TODO(wdang): determine if vcproj or sln
  string destination(".");
  if (argc > 2) {
    destination.assign(argv[2]);
  }
  
  // check output destination
  struct stat info;
  if (stat(destination.c_str(), &info) ==0) {
    if (info.st_mode & S_IFDIR) {
      destination.append("/Makefile");
    }
    else if (info.st_mode & S_IFREG) {
      destination.append(" is not a directory");
      return ErrorMessage(destination.c_str());
    }
  }
  else {
    destination.append(" is an invalid path.");
    return ErrorMessage(destination.c_str());
  }
  
  string errors;
  VCProject project(argv[1],&errors);
  if(!errors.empty()) {
    return ErrorMessage(errors);
  }
  Makefile makefile(project);

  // write the file
  ofstream outfile(destination.c_str());
  if (outfile.is_open()) {
    outfile << makefile.contents;
    outfile.close();
  }
  else {
    return ErrorMessage("Error writing Makefile");
  }
  printf("Output: %s\n", destination.c_str());
  fflush(stdout);

  return 0;
#endif
}
