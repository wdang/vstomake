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

#include <unordered_map>
#include <string>

// VCCLCompilerTool is for convienent access
// to the VCCLCompilerTool properties of a project configuration.
struct VCCLCompilerTool {

  // All possible enumeration values
  // used by VCCLCompilerTool
  enum Enum {
    Enum_Unknown,

    Enum_asmListingOption,
    Enum_asmListingNone,
    Enum_asmListingAssemblyOnly,
    Enum_asmListingAsmMachineSrc,
    Enum_asmListingAsmMachine,
    Enum_asmListingAsmSrc,

    Enum_basicRuntimeCheckOption,
    Enum_runtimeBasicCheckNone,
    Enum_runtimeCheckStackFrame,
    Enum_runtimeCheckUninitVariables,
    Enum_runtimeBasicCheckAll,

    Enum_browseInfoOption,
    Enum_brInfoNone,
    Enum_brAllInfo,
    Enum_brNoLocalSymbols,

    Enum_callingConventionOption,
    Enum_callConventionCDecl,
    Enum_callConventionFastCall,
    Enum_callConventionStdCall,

    Enum_CompileAsOptions,
    Enum_compileAsDefault,
    Enum_compileAsC,
    Enum_compileAsCPlusPlus,

    Enum_compileAsManagedOptions,
    Enum_managedNotSet,
    Enum_managedAssembly,
    Enum_managedAssemblyPure,
    Enum_managedAssemblySafe,
    Enum_managedAssemblyOldSyntax,

    Enum_debugOption,
    Enum_debugDisabled,
    Enum_debugOldStyleInfo,
    Enum_debugEnabled,
    Enum_debugEditAndContinue,

    Enum_enhancedInstructionSetType,
    Enum_enhancedInstructionSetTypeNotSet,
    Enum_enhancedInstructionSetTypeSIMD,
    Enum_enhancedInstructionSetTypeSIMD2,

    Enum_compilerErrorReportingType,
    Enum_compilerErrorReportingDefault,
    Enum_compilerErrorReportingPrompt,
    Enum_compilerErrorReportingQueue,

    Enum_cppExceptionHandling,
    Enum_cppExceptionHandlingNo,
    Enum_cppExceptionHandlingYes,
    Enum_cppExceptionHandlingYesWithSEH,

    Enum_favorSizeOrSpeedOption,
    Enum_favorNone,
    Enum_favorSpeed,
    Enum_favorSize,

    Enum_preprocessOption,
    Enum_preprocessNo,
    Enum_preprocessYes,
    Enum_preprocessNoLineNumbers,

    Enum_inlineExpansionOption,
    Enum_expandDisable,
    Enum_expandOnlyInline,
    Enum_expandAnySuitable,

    Enum_optimizeOption,
    Enum_optimizeDisabled,
    Enum_optimizeMinSpace,
    Enum_optimizeMaxSpeed,
    Enum_optimizeFull,
    Enum_optimizeCustom,

    Enum_runtimeLibraryOption,
    Enum_rtMultiThreaded,
    Enum_rtMultiThreadedDebug,
    Enum_rtMultiThreadedDLL,
    Enum_rtMultiThreadedDebugDLL,

    Enum_structMemberAlignOption,
    Enum_alignNotSet,
    Enum_alignSingleByte,
    Enum_alignTwoBytes,
    Enum_alignFourBytes,
    Enum_alignEightBytes,
    Enum_alignSixteenBytes,

    Enum_pchOption,
    Enum_pchNone,
    Enum_pchCreateUsingSpecific,
    Enum_pchUseUsingSpecific,

    Enum_warningLevelOption,
    Enum_warningLevel_0,
    Enum_warningLevel_1,
    Enum_warningLevel_2,
    Enum_warningLevel_3,
    Enum_warningLevel_4,

    Enum_floatingPointModel,
    Enum_FloatingPointPrecise,
    Enum_FloatingPointStrict,
    Enum_FloatingPointFast,
  };

  // Access VCCLCompilerTool properties from the given
  // project configuration
  
   
  // The following accessors return "" or Enum_Unknown or false 
  // to represent an empty value
  const char* AdditionalIncludeDirectories() const;
  const char* AdditionalOptions() const;
  const char* AdditionalUsingDirectories() const;
  const char* AssemblerListingLocation() const;
  Enum        AssemblerOutput() const;
  Enum        BasicRuntimeChecks() const;
  Enum        BrowseInformation() const;
  const char* BrowseInformationFile() const;
  bool        BufferSecurityCheck() const;
  Enum        CallingConvention() const;
  Enum        CompileAs() const;
  Enum        CompileAsManaged() const;
  bool        CompileOnly() const;
  Enum        DebugInformationFormat() const;
  bool        DefaultCharIsUnsigned() const;
  bool        Detect64BitPortabilityProblems() const;
  bool        DisableLanguageExtensions() const;
  const char* DisableSpecificWarnings() const;
  Enum        EnableEnhancedInstructionSet() const;
  bool        EnableFiberSafeOptimizations() const;
  bool        EnableFunctionLevelLinking() const;
  bool        EnableIntrinsicFunctions() const;
  bool        EnablePREfast() const;
  Enum        ErrorReporting() const;
  Enum        ExceptionHandling() const;
  const char* ExecutionBucket() const;
  bool        ExpandAttributedSource() const;
  Enum        FavorSizeOrSpeed() const;
  bool        FloatingPointExceptions() const;
  Enum        FloatingPointModel() const;
  bool        ForceConformanceInForLoopScope() const;
  const char* ForcedIncludeFiles() const;
  const char* ForcedUsingFiles() const;
  const char* FullIncludePath() const;
  Enum        GeneratePreprocessedFile() const;
  bool        GenerateXMLDocumentationFiles() const;
  bool        IgnoreStandardIncludePath() const;
  Enum        InlineFunctionExpansion() const;
  bool        KeepComments() const;
  bool        MinimalRebuild() const;
  const char* ObjectFile() const;
  const char* OmitDefaultLibName() const;
  bool        OmitFramePointers() const;
  bool        OpenMP() const;
  Enum        Optimization() const;
  const char* PrecompiledHeaderFile() const;
  const char* PrecompiledHeaderThrough() const;
  const char* PreprocessorDefinitions() const;
  const char* ProgramDataBaseFileName() const;
  Enum        RuntimeLibrary() const;
  bool        RuntimeTypeInfo() const;
  bool        ShowIncludes() const;
  bool        SmallerTypeCheck() const;
  bool        StringPooling() const;
  Enum        StructMemberAlignment() const;
  bool        SuppressStartupBanner() const;
  const char* ToolKind() const;
  const char* toolName() const;
  const char* ToolPath() const;
  bool        TreatWChar_tAsBuiltInType() const;
  bool        UndefineAllPreprocessorDefinitions() const;
  const char* UndefinePreprocessorDefinitions() const;
  const char* UseFullPaths() const;
  Enum        UsePrecompiledHeader() const;
  bool        UseUnicodeResponseFiles() const;
  const char* VCProjectEngine() const;
  bool        WarnAsError() const;
  Enum        WarningLevel() const;
  bool        WholeProgramOptimization() const;
  const char* XMLDocumentationFileName() const;

 private:
  friend struct VCProject;
  friend struct VCConfiguration;  
  VCCLCompilerTool();
  std::unordered_map<std::string, std::string>* properties;
};
