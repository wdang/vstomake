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
#ifndef LIBVS_HPP
#define LIBVS_HPP

#include <unordered_map>
#include <string>
#include <vector>

#include "vcclcompilertool.h"
namespace vs {

namespace Enum {

enum Encoding {
  Encoding_ASCII,
  Encoding_UTF8,
  Encoding_Unicode,
};

enum Format{
  Format_unknown = 0,
  Format_sln,
  Format_vcproj,
  Format_vcxproj,
  Format_csproj
};

enum CompileAs {
  CompileAs_Default,
  CompileAs_C,
  CompileAs_CPlusPlus,
};

enum AsmListing {
  AsmListing_None,
  AsmListing_AssemblyOnly,
  AsmListing_AsmMachineSrc,
  AsmListing_AsmMachine,
  AsmListing_AsmSrc,
};

enum BasicRuntimeCheck {
  BasicRuntimeCheck_None,
  BasicRuntimeCheck_StackFrame,
  BasicRuntimeCheck_UninitVariables,
  BasicRuntimeCheck_All,
};

enum BrowseInfo {
  BrowseInfo_InfoNone,
  BrowseInfo_AllInfo,
  BrowseInfo_NoLocalSymbols,
};

enum CallingConvention {
  CallingConvention_CDecl,
  CallingConvention_FastCall,
  CallingConvention_StdCall,
};

enum CompileAsManaged {
  CompileAsManaged_NotSet,
  CompileAsManaged_Assembly,
  CompileAsManaged_AssemblyPure,
  CompileAsManaged_AssemblySafe,
  CompileAsManaged_AssemblyOldSyntax,
};

enum CompilerErrorReporting {
  CompilerErrorReporting_Default,
  CompilerErrorReporting_Prompt,
  CompilerErrorReporting_Queue,
};

enum CppExceptionHandling {
  CppExceptionHandling_No,
  CppExceptionHandling_Yes,
  CppExceptionHandling_YesWithSEH,
};

enum Debug {
  Debug_Disabled,
  Debug_OldStyleInfo,
  Debug_Enabled,
  Debug_EditAndContinue,
};

enum EnhancedInstructionSet {
  EnhancedInstructionSet_NotSet,
  EnhancedInstructionSet_SIMD,
  EnhancedInstructionSet_SIMD2,
};

enum Favor {
  Favor_None,
  Favor_Speed,
  Favor_Size,
};

enum FloatingPoint {
  FloatingPoint_Precise,
  FloatingPoint_Strict,
  FloatingPoint_Fast,
};

enum Expand {
  Expand_Disable,
  Expand_OnlyInline,
  Expand_AnySuitable,
};

enum Optimize {
  Optimize_Disabled,
  Optimize_MinSpace,
  Optimize_MaxSpeed,
  Optimize_Full,
  Optimize_Custom,
};

enum Pch {
  Pch_None,
  Pch_CreateUsingSpecific,
  Pch_UseUsingSpecific,
};

enum Preprocess {
  Preprocess_No,
  Preprocess_Yes,
  Preprocess_NoLineNumbers,
};

enum Runtime {
  Runtime_MultiThreaded,
  Runtime_MultiThreadedDebug,
  Runtime_MultiThreadedDLL,
  Runtime_MultiTreadedDebugDLL,
};

enum Align {
  Align_NotSet,
  Align_SingleByte,
  Align_TwoBytes,
  Align_FourBytes,
  Align_EightBytes,
  Align_SixteenBytes,
};

enum WarningLevel {
  WarningLevel_0,
  WarningLevel_1,
  WarningLevel_2,
  WarningLevel_3,
  WarningLevel_4,
};

enum Type {
  Type_Unknown        = 0,
  Type_Application    = 1,
  Type_DynamicLibrary = 2,
  Type_StaticLibrary  = 4,
  Type_Utility        = 10,
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

}


struct ProjectItem {
  std::string     Name;
  ProjectItem*    Parent;
  struct Project* Project;
};

// Represents a filter folder in the project
struct Filter {
  std::vector<struct File*> Files;
  std::vector<Filter*>      Filters;
  std::string Name;  
};

// Represents a single file referenced by the vcproj
struct File {
  std::vector<std::string> Excluded;
  std::string Name;
  std::string RelativePath;
  std::string AbsolutePath;  
  bool        ForcedInclude;
  bool        Precompiled;
  bool        CompileAsC;
};


// Represents a project build configuration
struct Configuration {

  std::unordered_map<std::string, std::unordered_map<std::string, std::string> > ToolProperties;
  
  // Files  specific to this configuration that are not excluded from build
  std::vector<File*> Files;

  // Path to inherited property sheets(.vsprops or .props)
  std::vector<std::string> PropertySheets;
  
  // "Debug|Win32", "Release|Win32" etc.
  std::string Name;  
  
  // "Debug","Release" etc.
  std::string ConfigurationName;

  // Path for intermediate build artifacts
  std::string IntermediateDirectory;

  // Path for final build target
  std::string OutputDirectory;

  // "Win32",  "x64" etc.
  std::string Platform;

  Enum::CharSet          CharacterSet;
  Enum::Type             ConfigurationType;
  Enum::UseATL           UseOfATL;
  Enum::UseMfc           UseOfMfc;
  Enum::Optimization     WholeProgramOptimization;
  VCCLCompilerTool CLCompilerTool;
};


// Represents a VCProject file
// VCProject parses a .vcproj on construction
struct Project{
  
  // Parse the project file located at the given filepath 
  // into ptr.
  // 
  // @filepath  location of the project file
  // @ptr       pointer to user allocated Project structure
  // @status    pointer to user allocated string to hold error messages(optional)
  // 
  // If parsing succeeds this function returns true with status as an empty string else
  // status will contain a relevent error message
  static bool Parse(const std::string& filepath, Project* ptr, std::string* status = 0);
  
  
  // All files referenced by the project
  std::vector<File> Files;

  // All filters as displayed in the solution explorer
  std::vector<Filter> Filters;

  // All platforms the project targets
  std::vector<std::string> Platforms;

  // All build configurations
  std::vector<Configuration> Configurations;

  // Full file path of the .vcproj file
  std::string AbsolutePath;

  // Name of the project as displayed in the solution explorer
  std::string Name;

  // .vcproj Path this vsProject was instantiated with
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

}
#endif //LIBVS_HPP
