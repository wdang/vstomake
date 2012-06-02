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
#include "vcclcompilertool.h"

// VCCLCompilerTool constructor implementation
VCCLCompilerTool::VCCLCompilerTool(const VCProject::Configuration& configuration) {
  auto iter = configuration.properties.find("VCCLCompilerTool");
  if (iter != configuration.properties.end()) {
    properties = iter->second;
  }
}


VCCLCompilerTool::Enum GetEnum(const std::string&){return VCCLCompilerTool::Enum_Unknown;}
// Implemented via excessive X-macros ;)
// Implemention for members returning a VCCLCompilerTool::Enum 
#define VCCLCOMPILERTOOL_ENUM_ACCESSORS X(AssemblerOutput)\
X(BasicRuntimeChecks)\
X(BrowseInformation)\
X(CallingConvention)\
X(CompileAs)\
X(CompileAsManaged)\
X(DebugInformationFormat)\
X(EnableEnhancedInstructionSet)\
X(ErrorReporting)\
X(ExceptionHandling)\
X(FavorSizeOrSpeed)\
X(GeneratePreprocessedFile)\
X(InlineFunctionExpansion)\
X(Optimization)\
X(RuntimeLibrary)\
X(StructMemberAlignment)\
X(UsePrecompiledHeader)\
X(WarningLevel)\
X(floatingPointModel)

#define X(NAME) VCCLCompilerTool::Enum VCCLCompilerTool::NAME() const {\
  auto iter = properties.find(#NAME);\
  VCCLCompilerTool::Enum rv;\
  if(iter != properties.end()){\
     rv = GetEnum(iter->second);\
  }else{\
    rv = Enum_Unknown;\
  }\
  return rv;\
}
VCCLCOMPILERTOOL_ENUM_ACCESSORS
#undef X



// Implemention for members returning a bool
#define VCCLCOMPILERTOOL_BOOL_ACCESSORS X(BufferSecurityCheck)\
X(CompileOnly)\
X(DefaultCharIsUnsigned)\
X(Detect64BitPortabilityProblems)\
X(DisableLanguageExtensions)\
X(EnableFiberSafeOptimizations)\
X(EnableFunctionLevelLinking)\
X(EnableIntrinsicFunctions)\
X(EnablePREfast)\
X(ExpandAttributedSource)\
X(FloatingPointExceptions)\
X(ForceConformanceInForLoopScope)\
X(GenerateXMLDocumentationFiles)\
X(IgnoreStandardIncludePath)\
X(KeepComments)\
X(MinimalRebuild)\
X(OmitFramePointers)\
X(RuntimeTypeInfo)\
X(ShowIncludes)\
X(SmallerTypeCheck)\
X(StringPooling)\
X(TreatWChar_tAsBuiltInType)\
X(UndefineAllPreprocessorDefinitions)\
X(UseUnicodeResponseFiles)\
X(WarnAsError)\
X(WholeProgramOptimization)

#define X(NAME) bool VCCLCompilerTool::NAME() const {\
  auto iter = properties.find(#NAME);\
  bool rv = false;\
  if(iter != properties.end()){\
    std::string value(iter->second);\
    std::transform(value.begin(),value.end(),value.begin(),tolower);\
    rv = (value.find("true") != std::string::npos);\
  }\
  return rv;\
}
VCCLCOMPILERTOOL_BOOL_ACCESSORS
#undef X



// Implemention for members returning a string
#define VCCLCOMPILERTOOL_STRING_ACCESSORS X(AdditionalIncludeDirectories)\
X(AdditionalOptions)\
X(AdditionalUsingDirectories)\
X(AssemblerListingLocation)\
X(BrowseInformationFile)\
X(DisableSpecificWarnings)\
X(ExecutionBucket)\
X(ForcedIncludeFiles)\
X(ForcedUsingFiles)\
X(FullIncludePath)\
X(ObjectFile)\
X(OmitDefaultLibName)\
X(OpenMP)\
X(PrecompiledHeaderFile)\
X(PrecompiledHeaderThrough)\
X(PreprocessorDefinitions)\
X(ProgramDataBaseFileName)\
X(SuppressStartupBanner)\
X(ToolKind)\
X(ToolPath)\
X(UndefinePreprocessorDefinitions)\
X(UseFullPaths)\
X(VCProjectEngine)\
X(XMLDocumentationFileName)\
X(toolName)
#define X(NAME) const char* VCCLCompilerTool::NAME() const { \
    auto iter = properties.find(# NAME); \
    return iter == properties.end() ? "" : iter->second.c_str(); \
}
VCCLCOMPILERTOOL_STRING_ACCESSORS
#undef X
