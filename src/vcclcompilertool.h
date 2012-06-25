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

namespace vs {
// VCCLCompilerTool is for convienent access
// to the VCCLCompilerTool properties of a project configuration.
struct VCCLCompilerTool {
  struct Enum {
    enum asmListingOption {
      asmListingNone,
      asmListingAssemblyOnly,
      asmListingAsmMachineSrc,
      asmListingAsmMachine,
      asmListingAsmSrc
    };

    enum basicRuntimeCheckOption {
      runtimeBasicCheckNone,
      runtimeCheckStackFrame,
      runtimeCheckUninitVariables,
      runtimeBasicCheckAll
    };

    enum browseInfoOption {
      brInfoNone,
      brAllInfo,
      brNoLocalSymbols
    };

    enum callingConventionOption {
      callConventionCDecl,
      callConventionFastCall,
      callConventionStdCall
    };

    enum CompileAsOptions {
      compileAsDefault,
      compileAsC,
      compileAsCPlusPlus
    };

    enum compileAsManagedOptions {
      managedNotSet,
      managedAssembly,
      managedAssemblyPure,
      managedAssemblySafe,
      managedAssemblyOldSyntax
    };

    enum debugOption {
      debugDisabled,
      debugOldStyleInfo,
      debugEnabled,
      debugEditAndContinue
    };

    enum enhancedInstructionSetType {
      enhancedInstructionSetTypeNotSet,
      enhancedInstructionSetTypeSIMD,
      enhancedInstructionSetTypeSIMD2
    };

    enum compilerErrorReportingType {
      compilerErrorReportingDefault,
      compilerErrorReportingPrompt,
      compilerErrorReportingQueue
    };

    enum cppExceptionHandling {
      cppExceptionHandlingNo,
      cppExceptionHandlingYes,
      cppExceptionHandlingYesWithSEH
    };

    enum favorSizeOrSpeedOption {
      favorNone,
      favorSpeed,
      favorSize
    };

    enum floatingPointModel {
      FloatingPointPrecise,
      FloatingPointStrict,
      FloatingPointFast,
    };

    enum preprocessOption {
      preprocessNo,
      preprocessYes,
      preprocessNoLineNumbers
    };
    
    enum inlineExpansionOption {
      expandDisable,
      expandOnlyInline,
      expandAnySuitable
    };
    
    enum optimizeOption {
      optimizeDisabled,
      optimizeMinSpace,
      optimizeMaxSpeed,
      optimizeFull,
      optimizeCustom
    };
    
    enum runtimeLibraryOption {
      rtMultiThreaded,
      rtMultiThreadedDebug,
      rtMultiThreadedDLL,
      rtMultiThreadedDebugDLL
    };
    
    enum structMemberAlignOption {
      alignNotSet,
      alignSingleByte,
      alignTwoBytes,
      alignFourBytes,
      alignEightBytes,
      alignSixteenBytes
    };
    
    enum pchOption {
      pchNone,
      pchCreateUsingSpecific,
      pchUseUsingSpecific
    };
    
    enum warningLevelOption {
      warningLevel_0,
      warningLevel_1,
      warningLevel_2,
      warningLevel_3,
      warningLevel_4
    };
  };

  const char*                   AdditionalIncludeDirectories() const;
  const char*                   AdditionalOptions() const;
  const char*                   AdditionalUsingDirectories() const;
  const char*                   AssemblerListingLocation() const;
  Enum::asmListingOption        AssemblerOutput() const;
  Enum::basicRuntimeCheckOption BasicRuntimeChecks() const;
  Enum::browseInfoOption        BrowseInformation() const;
  const char*                   BrowseInformationFile() const;
  bool                          BufferSecurityCheck() const;
  Enum::callingConventionOption CallingConvention() const;
  Enum::CompileAsOptions        CompileAs() const;
  Enum::compileAsManagedOptions CompileAsManaged() const;
  bool                             CompileOnly() const;
  Enum::debugOption                DebugInformationFormat() const;
  bool                             DefaultCharIsUnsigned() const;
  bool                             Detect64BitPortabilityProblems() const;
  bool                             DisableLanguageExtensions() const;
  const char*                      DisableSpecificWarnings() const;
  Enum::enhancedInstructionSetType EnableEnhancedInstructionSet() const;
  bool                             EnableFiberSafeOptimizations() const;
  bool                             EnableFunctionLevelLinking() const;
  bool                             EnableIntrinsicFunctions() const;
  bool                             EnablePREfast() const;
  Enum::compilerErrorReportingType ErrorReporting() const;
  Enum::cppExceptionHandling       ExceptionHandling() const;
  const char*                      ExecutionBucket() const;
  bool                             ExpandAttributedSource() const;
  Enum::favorSizeOrSpeedOption     FavorSizeOrSpeed() const;
  bool                             FloatingPointExceptions() const;
  Enum::floatingPointModel         FloatingPointModel() const;
  bool                             ForceConformanceInForLoopScope() const;
  const char*                      ForcedIncludeFiles() const;
  const char*                      ForcedUsingFiles() const;
  const char*                      FullIncludePath() const;
  Enum::preprocessOption           GeneratePreprocessedFile() const;
  bool                             GenerateXMLDocumentationFiles() const;
  bool                             IgnoreStandardIncludePath() const;
  Enum::inlineExpansionOption      InlineFunctionExpansion() const;
  bool                             KeepComments() const;
  bool                             MinimalRebuild() const;
  const char*                      ObjectFile() const;
  const char*                      OmitDefaultLibName() const;
  bool                             OmitFramePointers() const;
  bool                             OpenMP() const;
  Enum::optimizeOption             Optimization() const;
  const char*                      PrecompiledHeaderFile() const;
  const char*                      PrecompiledHeaderThrough() const;
  const char*                      PreprocessorDefinitions() const;
  const char*                      ProgramDataBaseFileName() const;
  Enum::runtimeLibraryOption       RuntimeLibrary() const;
  bool                             RuntimeTypeInfo() const;
  bool                             ShowIncludes() const;
  bool                             SmallerTypeCheck() const;
  bool                             StringPooling() const;
  Enum::structMemberAlignOption    StructMemberAlignment() const;
  bool                             SuppressStartupBanner() const;
  const char*                      ToolKind() const;
  const char*                      toolName() const;
  const char*                      ToolPath() const;
  bool                             TreatWChar_tAsBuiltInType() const;
  bool                             UndefineAllPreprocessorDefinitions() const;
  const char*                      UndefinePreprocessorDefinitions() const;
  const char*                      UseFullPaths() const;
  Enum::pchOption                  UsePrecompiledHeader() const;
  bool                             UseUnicodeResponseFiles() const;
  const char*                      VCProjectEngine() const;
  bool                             WarnAsError() const;
  Enum::warningLevelOption         WarningLevel() const;
  bool                             WholeProgramOptimization() const;
  const char*                      XMLDocumentationFileName() const;


private:
  friend struct Project;
  friend struct Configuration;
  VCCLCompilerTool();
  const std::unordered_map<std::string, std::string>* properties;
};
}




