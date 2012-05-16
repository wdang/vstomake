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
#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct VCProject {

  // Configuration type
  enum Type {
    Type_Invalid,
    Type_Application,
    Type_DynamicLibrary,
    Type_Generic,
    Type_StaticLibrary,
    Type_Utility
  };

  // a file referenced by the project
  struct File {
    std::unordered_map<std::string, std::string> properties;
    std::string                                  path;
  };

  // a build configuration
  struct Configuration {
    std::vector<VCProject::File> files;
    std::vector<std::string>     excluded_files;
    std::vector<std::string>     forced_includes;

    //tool properties
    std::unordered_map<std::string, std::unordered_map<std::string, std::string> > properties;

    std::string                                                                    name;
    std::string                                                                    platform;
    std::string                                                                    output_dir;
    std::string                                                                    intermediate_dir;
    std::string                                                                    vsprops;
    std::string                                                                    precompiled_header;
    VCProject::Type                                                                type;
  };

  // Construct a VCProject object
  // and parses the given string as a vcproj
  //
  // @contents: in memory contents of a .vcproj file
  explicit VCProject(const std::string& contents);

  // members are populated on construction
  std::vector<Configuration> configurations;
  std::string                guid;
  std::string                name;
  std::string                path;
  std::string                version;
};
