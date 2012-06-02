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
#include "vcproject.h"

// VCCLCompilerTool is for convienent access
// to the VCCLCompilerTool properties of a project configuration.
struct VCCLCompilerTool {
  
  // All possible enumeration values
  // used by VCCLCompilerTool
  enum Enum {
    Enum_Unknown,
    Enum_asmListingNone,
    Enum_asmListingAssemblyOnly,
    Enum_asmListingAsmMachineSrc,
    Enum_asmListingAsmMachine,
    Enum_asmListingAsmSrc,
    Enum_runtimeBasicCheckNone,
    Enum_runtimeCheckStackFrame,
    Enum_runtimeCheckUninitVariables,
    Enum_runtimeBasicCheckAll,
    Enum_brInfoNone,
    Enum_brAllInfo,
    Enum_brNoLocalSymbols,
    Enum_managedNotSet,
    Enum_managedAssembly,
    Enum_managedAssemblyPure,
    Enum_managedAssemblySafe,
    Enum_managedAssemblyOldSyntax,
    Enum_cppExceptionHandlingNo,
    Enum_cppExceptionHandlingYes,
    Enum_cppExceptionHandlingYesWithSEH,
    Enum_debugDisabled,
    Enum_debugOldStyleInfo,
    Enum_debugEnabled,
    Enum_debugEditAndContinue,
    Enum_enhancedInstructionSetTypeNotSet,
    Enum_enhancedInstructionSetTypeSIMD,
    Enum_enhancedInstructionSetTypeSIMD2,
    Enum_optimizeDisabled,
    Enum_optimizeMinSpace,
    Enum_optimizeMaxSpeed,
    Enum_optimizeFull,
    Enum_optimizeCustom,
    Enum_pchNone,
    Enum_pchCreateUsingSpecific,
    Enum_pchUseUsingSpecific,
    Enum_preprocessNo,
    Enum_preprocessYes,
    Enum_preprocessNoLineNumbers,
    Enum_rtMultiThreaded,
    Enum_rtMultiThreadedDebug,
    Enum_rtMultiThreadedDLL,
    Enum_rtMultiThreadedDebugDLL,
    Enum_alignNotSet,
    Enum_alignSingleByte,
    Enum_alignTwoBytes,
    Enum_alignFourBytes,
    Enum_alignEightBytes,
    Enum_alignSixteenBytes
  };
  
  // Access VCCLCompilerTool properties from the given
  // project configuration
  explicit VCCLCompilerTool(const VCProject::Configuration& configuration);
  
  // The following accessors return "" to represent an empty value
  const char* AdditionalIncludeDirectories() const;
  const char* AdditionalOptions() const;
  const char* AdditionalUsingDirectories() const;
  const char* AssemblerListingLocation() const;
  Enum AssemblerOutput() const;
  Enum BasicRuntimeChecks() const;
  Enum BrowseInformation() const;
  const char* BrowseInformationFile() const;
  bool BufferSecurityCheck() const;
  Enum CallingConvention() const;
  Enum CompileAs() const;
  Enum CompileAsManaged() const;
  bool CompileOnly() const;
  Enum DebugInformationFormat() const;
  bool DefaultCharIsUnsigned() const;
  bool Detect64BitPortabilityProblems() const;
  bool DisableLanguageExtensions() const;
  const char* DisableSpecificWarnings() const;
  Enum EnableEnhancedInstructionSet() const;
  bool EnableFiberSafeOptimizations() const;
  bool EnableFunctionLevelLinking() const;
  bool EnableIntrinsicFunctions() const;
  bool EnablePREfast() const;
  Enum ErrorReporting() const;
  Enum ExceptionHandling() const;
  const char* ExecutionBucket() const;
  bool ExpandAttributedSource() const;
  Enum FavorSizeOrSpeed() const;
  bool FloatingPointExceptions() const;
  Enum floatingPointModel() const;
  bool ForceConformanceInForLoopScope() const;
  const char* ForcedIncludeFiles() const;
  const char* ForcedUsingFiles() const;
  const char* FullIncludePath() const;
  Enum GeneratePreprocessedFile() const;
  bool GenerateXMLDocumentationFiles() const;
  bool IgnoreStandardIncludePath() const;
  Enum InlineFunctionExpansion() const;
  bool KeepComments() const;
  bool MinimalRebuild() const;
  const char* ObjectFile() const;
  const char* OmitDefaultLibName() const;
  bool OmitFramePointers() const;
  const char* OpenMP() const;
  Enum Optimization() const;
  const char* PrecompiledHeaderFile() const;
  const char* PrecompiledHeaderThrough() const;
  const char* PreprocessorDefinitions() const;
  const char* ProgramDataBaseFileName() const;
  Enum RuntimeLibrary() const;
  bool RuntimeTypeInfo() const;
  bool ShowIncludes() const;
  bool SmallerTypeCheck() const;
  bool StringPooling() const;
  Enum StructMemberAlignment() const;
  const char* SuppressStartupBanner() const;
  const char* ToolKind() const;
  const char* toolName() const;
  const char* ToolPath() const;
  bool TreatWChar_tAsBuiltInType() const;
  bool UndefineAllPreprocessorDefinitions() const;
  const char* UndefinePreprocessorDefinitions() const;
  const char* UseFullPaths() const;
  Enum UsePrecompiledHeader() const;
  bool UseUnicodeResponseFiles() const;
  const char* VCProjectEngine() const;
  bool WarnAsError() const;
  Enum WarningLevel() const;
  bool WholeProgramOptimization() const;
  const char* XMLDocumentationFileName() const;

 private:
  std::unordered_map<std::string, std::string> properties;
};
