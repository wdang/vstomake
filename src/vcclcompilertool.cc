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
#include "utility.h"

VCCLCompilerTool::VCCLCompilerTool(){}

// VCCLCompilerTool's members is implemented via excessive X-macros
// since their functionality simply queries the underlying property map
#define VCCLCOMPILERTOOL_ENUM_ACCESSORS X(AssemblerOutput, asmListingOption, 5)\
X(BasicRuntimeChecks, basicRuntimeCheckOption, 4)\
X(BrowseInformation, browseInfoOption, 3)\
X(CallingConvention, callingConventionOption, 3)\
X(CompileAs, CompileAsOptions, 3)\
X(CompileAsManaged, compileAsManagedOptions, 5)\
X(DebugInformationFormat, debugOption, 4)\
X(EnableEnhancedInstructionSet, enhancedInstructionSetType, 3)\
X(ErrorReporting, compilerErrorReportingType, 3)\
X(ExceptionHandling, cppExceptionHandling, 3)\
X(FavorSizeOrSpeed, favorSizeOrSpeedOption, 3)\
X(GeneratePreprocessedFile, preprocessOption, 3)\
X(InlineFunctionExpansion, inlineExpansionOption, 3)\
X(Optimization, optimizeOption, 5)\
X(RuntimeLibrary, runtimeLibraryOption, 4)\
X(StructMemberAlignment, structMemberAlignOption, 6)\
X(UsePrecompiledHeader, pchOption, 3)\
X(WarningLevel, warningLevelOption, 5)\
X(FloatingPointModel, floatingPointModel, 3)

// Holds information about a VCCLCompilerTool property,
// it's enumeration type, and its maximum possible value.
struct PropertyInfo {
  const char* name;
  VCCLCompilerTool::Enum type;
  int         max;
};

// kVCCLCompilerPropertyInfo holds all properties that
// return an enumeration value
#define X(NAME, TYPE, LEN) {# NAME, VCCLCompilerTool::Enum_ ## TYPE, LEN},
static PropertyInfo kVCCLCompilerPropertyInfo[] = {
  VCCLCOMPILERTOOL_ENUM_ACCESSORS
};
#undef X

// Returns the enumeration associated wth the specified
// VCCLCompilerTool property and it's associated value.
//
// The arguments given to GetEnum will be straight
// from the parsed vcproj file.
//
// @name  VCCLCompilerTool property name
// @value VCCLCompilerTool property value
//
// returns VCCLCompilerTool::Enum_Unknown if the property's value is not set
VCCLCompilerTool::Enum GetEnum(const std::string& name, const std::string& value) {
  VCCLCompilerTool::Enum rv = VCCLCompilerTool::Enum_Unknown;
  
  for(size_t i = 0; i < ARRAY_COUNT(kVCCLCompilerPropertyInfo); ++i) {
    PropertyInfo& prop = kVCCLCompilerPropertyInfo[i];
    
    if(name.compare(prop.name) == 0) {
      // Since we shove all enumerations in a giant
      // definition (VCCLCompilerTool::Enum) instead separately, we simply
      // use an offset from PropertyInfo.type to obtain the correct Enum
      // representation of the 'value'. The offset is incremented because
      // all property values are zero-based
      int offset = strtol(value.c_str(), 0, 10) + 1;
      
      if(offset <= prop.max + 1) {
        // For some reason the DebugInformationFormat property
        // uses the following values: [0,1,3,4]
        // instead of: [0,1,2,3] so theres going to be exceptions
        // in incrementing the offset.
        if(prop.type == VCCLCompilerTool::Enum_debugOption && offset >= 3)
          --offset;
        
        rv = static_cast<VCCLCompilerTool::Enum>(prop.type + offset);
        break;
      }
      
    }
    
  }
  
  return rv;
}

#define X(NAME, TYPE, LEN) VCCLCompilerTool::Enum VCCLCompilerTool::NAME() const { \
    auto iter = properties->find(# NAME);                                           \
    VCCLCompilerTool::Enum rv;                                                     \
    if (iter != properties->end()) {                                                \
      rv = GetEnum(# NAME, iter->second);                                          \
    }else {                                                                        \
      rv = VCCLCompilerTool::Enum_Unknown;                                         \
    }                                                                              \
    return rv;                                                                     \
}
VCCLCOMPILERTOOL_ENUM_ACCESSORS
#undef X


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
X(OpenMP)\
X(RuntimeTypeInfo)\
X(ShowIncludes)\
X(SmallerTypeCheck)\
X(SuppressStartupBanner)\
X(StringPooling)\
X(TreatWChar_tAsBuiltInType)\
X(UndefineAllPreprocessorDefinitions)\
X(UseUnicodeResponseFiles)\
X(WarnAsError)\
X(WholeProgramOptimization)

#define X(NAME) bool VCCLCompilerTool::NAME() const { \
    auto iter = properties->find(# NAME); \
    bool rv   = false; \
    if (iter != properties->end()) { \
      std::string value(iter->second); \
      std::transform(value.begin(), value.end(), value.begin(), tolower); \
      rv = (value.find("true") != std::string::npos); \
    } \
    return rv; \
}
VCCLCOMPILERTOOL_BOOL_ACCESSORS
#undef X


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
X(PrecompiledHeaderFile)\
X(PrecompiledHeaderThrough)\
X(PreprocessorDefinitions)\
X(ProgramDataBaseFileName)\
X(ToolKind)\
X(ToolPath)\
X(UndefinePreprocessorDefinitions)\
X(UseFullPaths)\
X(VCProjectEngine)\
X(XMLDocumentationFileName)\
X(toolName)
#define X(NAME) const char* VCCLCompilerTool::NAME() const { \
    auto iter = properties->find(# NAME); \
    return iter == properties->end() ? "" : iter->second.c_str(); \
}

VCCLCOMPILERTOOL_STRING_ACCESSORS
#undef X
