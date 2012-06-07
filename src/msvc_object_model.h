// Copyright 2012 William Dang.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

// "Pseudo" representaton of a few Visual Studio
// automation object model interfaces. 
//
// VCProject should be the only structure used in client code.
#include <vector>
#include <iosfwd>

#include "vcclcompilertool.h"

// Represents a filter folder in the project
struct VCFilter {
  std::vector<struct VCFile*> Files;
  std::string Name;
  VCFilter*   Parent;
  std::vector<VCFilter*> Filters;
};

// Represents a single file referenced by the vcproj
struct VCFile {
  VCFile();
  std::string Filename;
  std::string RelativePath;
  std::string AbsolutePath;
  std::string Filter;
  std::vector<std::string> Excluded;
  bool        ForcedInclude;
  bool        PrecompiledHeader;
  bool        CompileAsC;
};

// Represents a project build configuration
struct VCConfiguration {

  enum Type {
    Type_Unknown,
    Type_Application,
    Type_DynamicLibrary,
    Type_StaticLibrary,
    Type_Generic,
  };

  enum Optimization {
    Optimization_None,
    Optimization_LinkTimeCodeGen,
    Optimization_PGOInstrument,
    Optimization_PGOOptimize,
    Optimization_PGOUpdate
  };


  enum CharSet {
    CharSet_NotSet,
    CharSet_Unicode,
    CharSet_MBCS,
  };

  enum UseATL {
    UseATL_NotSet,
    UseATL_Static,
    UseATL_Dynamic,
  };

  enum UseMfc {
    UseMfc_StdWin,
    UseMfc_Static,
    UseMfc_Dynamic,
  };
  
  std::unordered_map<std::string,std::unordered_map<std::string, std::string>> ToolProperties;  
  
  // VCFiles that are !ExcludedFromBuild
  std::vector<VCFile*> Files;

  // "Debug","Release" etc.
  std::string ConfigurationName;

  // Path for intermediate build artifacts
  std::string IntermediateDirectory;

  // "Debug|Win32", "Release|Win32" etc.
  std::string Name;

  // Path for final build target
  std::string OutputDirectory;

  // "Win32" "x64" etc.
  std::string Platform;

  // Path to inherited property sheet(.vsprops)
  std::string  PropertySheets;

  CharSet      CharacterSet;
  Type         ConfigurationType;
  UseATL       UseOfATL;
  UseMfc       UseOfMfc;
  Optimization WholeProgramOptimization;  
  VCCLCompilerTool CLCompilerTool;
};

// Represents a VCProject file
// VCProject parses a .vcproj on construction
struct VCProject {


  // Initialize a VCProject and parse the given .vcproj file
  //
  // @path: relative or absolute path to a .vcproj file
  // @errors: error code return value (not-implemented)
  explicit VCProject(const std::string& path, std::string* errors = 0);

  // Initialize a VCProject and process given .vcproj file
  // using the specified parser
  //
  // @path: relative or absolute path to a .vcproj file
  // @parser: user defined parser for processing a project file
  // @errors: error code return value (not-implemented)
  explicit VCProject(const std::string& path, struct ProjectParser* parser, std::string* errors = 0);

  // All files referenced by the project
  std::vector<VCFile> Files;

  // All filters as displayed in the solution explorer
  std::vector<VCFilter> Filters;

  // All platforms the project targets
  std::vector<std::string> Platforms;

  // All build configurations
  std::vector<VCConfiguration> Configurations;

  // Full file path of the .vcproj file
  std::string AbsolutePath;

  // Name of the project as displayed in the solution explorer
  std::string Name;

  // .vcproj Path this VCProject was instantiated with
  std::string Path;

  // The following members represent the attributes of the
  // VisualStudioProject node in the vcproj file
  std::string ProjectType;
  std::string ProjectGUID;
  std::string RootNamespace;
  std::string Keyword;
  std::string TargetFrameworkVersion;

  // Full directory path containing the .vcproj file
  // without a trailing slash
  std::string ProjectDirectory;

  // Name + .vcproj
  std::string ProjectFile;

  // "9.00" for VS2008 "10.00" for VS2010
  std::string Version;
};
