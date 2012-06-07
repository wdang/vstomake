#pragma once

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


// volatile internal header
#include <string>
#include <unordered_map>

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>

struct VCFile;
void FilterCPPSources(const std::vector<VCFile*>& sources, std::vector<VCFile*>* out);


template<class Generator>
extern void GenerateMakefile(const Generator& gen);

// In place-expansion of macros(see kProjectMacros) found in input
void ExpandMacros(std::string* input, 
   std::unordered_map<std::string,std::string>& macros);

// Expand macros(see kProjectMacros) found in input
//
// @input: target for macro expansion
// @macros: definitions used for macro expansion
//
// Returns a copy of input with macros expanded
std::string ExpandMacros(const std::string& input, 
   std::unordered_map<std::string,std::string>& macros);

namespace internal {
   enum CompileAsOptions {
    compileAsDefault,
    compileAsC,
    compileAsCPlusPlus,
  };

  enum asmListingOption {
    asmListingNone,
    asmListingAssemblyOnly,
    asmListingAsmMachineSrc,
    asmListingAsmMachine,
    asmListingAsmSrc,
  };

  enum basicRuntimeCheckOption {
    runtimeBasicCheckNone,
    runtimeCheckStackFrame,
    runtimeCheckUninitVariables,
    runtimeBasicCheckAll,
  };

  enum browseInfoOption {
    brInfoNone,
    brAllInfo,
    brNoLocalSymbols,
  };

  enum callingConventionOption {
    callConventionCDecl,
    callConventionFastCall,
    callConventionStdCall,
  };

  enum compileAsManagedOptions {
    managedNotSet,
    managedAssembly,
    managedAssemblyPure,
    managedAssemblySafe,
    managedAssemblyOldSyntax,
  };

  enum compilerErrorReportingType {
    compilerErrorReportingDefault,
    compilerErrorReportingPrompt,
    compilerErrorReportingQueue,
  };

  enum cppExceptionHandling {
    cppExceptionHandlingNo,
    cppExceptionHandlingYes,
    cppExceptionHandlingYesWithSEH,
  };

  enum debugOption {
    debugDisabled,
    debugOldStyleInfo,
    debugEnabled,
    debugEditAndContinue,
  };

  enum enhancedInstructionSetType {
    enhancedInstructionSetTypeNotSet,
    enhancedInstructionSetTypeSIMD,
    enhancedInstructionSetTypeSIMD2,
  };

  enum favorSizeOrSpeedOption {
    favorNone,
    favorSpeed,
    favorSize,
  };

  enum floatingPointModel {
    FloatingPointPrecise,
    FloatingPointStrict,
    FloatingPointFast,
  };

  enum inlineExpansionOption {
    expandDisable,
    expandOnlyInline,
    expandAnySuitable,
  };

  enum optimizeOption {
    optimizeDisabled,
    optimizeMinSpace,
    optimizeMaxSpeed,
    optimizeFull,
    optimizeCustom,
  };

  enum pchOption {
    pchNone,
    pchCreateUsingSpecific,
    pchUseUsingSpecific,
  };

  enum preprocessOption {
    preprocessNo,
    preprocessYes,
    preprocessNoLineNumbers,
  };

  enum runtimeLibraryOption {
    rtMultiThreaded,
    rtMultiThreadedDebug,
    rtMultiThreadedDLL,
    rtMultiThreadedDebugDLL,
  };

  enum structMemberAlignOption {
    alignNotSet,
    alignSingleByte,
    alignTwoBytes,
    alignFourBytes,
    alignEightBytes,
    alignSixteenBytes,
  };

  enum warningLevelOption {
    warningLevel_0,
    warningLevel_1,
    warningLevel_2,
    warningLevel_3,
    warningLevel_4,
  };
}

