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

void InitVCCLCompilerToolProperties(std::unordered_map<std::string, std::string>* out);

struct ToolConfiguration;
struct VCCLCompilerTool {
  VCCLCompilerTool(const std::unordered_map<std::string, std::string>& tool_properties);
  VCCLCompilerTool() {}

  const char* AdditionalIncludeDirectories() const;
  const char* AdditionalOptions() const;
  const char* AdditionalUsingDirectories() const;
  const char* AssemblerListingLocation() const;
  const char* AssemblerOutput() const;
  const char* BasicRuntimeChecks() const;
  const char* BrowseInformation() const;
  const char* BrowseInformationFile() const;
  const char* BufferSecurityCheck() const;
  const char* CallingConvention() const;
  const char* CompileAs() const;
  const char* CompileAsManaged() const;
  const char* CompileOnly() const;
  const char* DebugInformationFormat() const;
  const char* DefaultCharIsUnsigned() const;
  const char* Detect64BitPortabilityProblems() const;
  const char* DisableLanguageExtensions() const;
  const char* DisableSpecificWarnings() const;
  const char* EnableEnhancedInstructionSet() const;
  const char* EnableFiberSafeOptimizations() const;
  const char* EnableFunctionLevelLinking() const;
  const char* EnableIntrinsicFunctions() const;
  const char* EnablePREfast() const;
  const char* ErrorReporting() const;
  const char* ExceptionHandling() const;
  const char* ExecutionBucket() const;
  const char* ExpandAttributedSource() const;
  const char* FavorSizeOrSpeed() const;
  const char* FloatingPointExceptions() const;
  const char* floatingPointModel() const;
  const char* ForceConformanceInForLoopScope() const;
  const char* ForcedIncludeFiles() const;
  const char* ForcedUsingFiles() const;
  const char* FullIncludePath() const;
  const char* GeneratePreprocessedFile() const;
  const char* GenerateXMLDocumentationFiles() const;
  const char* IgnoreStandardIncludePath() const;
  const char* InlineFunctionExpansion() const;
  const char* KeepComments() const;
  const char* MinimalRebuild() const;
  const char* ObjectFile() const;
  const char* OmitDefaultLibName() const;
  const char* OmitFramePointers() const;
  const char* OpenMP() const;
  const char* Optimization() const;
  const char* PrecompiledHeaderFile() const;
  const char* PrecompiledHeaderThrough() const;
  const char* PreprocessorDefinitions() const;
  const char* ProgramDataBaseFileName() const;
  const char* RuntimeLibrary() const;
  const char* RuntimeTypeInfo() const;
  const char* ShowIncludes() const;
  const char* SmallerTypeCheck() const;
  const char* StringPooling() const;
  const char* StructMemberAlignment() const;
  const char* SuppressStartupBanner() const;
  const char* ToolKind() const;
  const char* toolName() const;
  const char* ToolPath() const;
  const char* TreatWChar_tAsBuiltInType() const;
  const char* UndefineAllPreprocessorDefinitions() const;
  const char* UndefinePreprocessorDefinitions() const;
  const char* UseFullPaths() const;
  const char* UsePrecompiledHeader() const;
  const char* UseUnicodeResponseFiles() const;
  const char* VCProjectEngine() const;
  const char* WarnAsError() const;
  const char* WarningLevel() const;
  const char* WholeProgramOptimization() const;
  const char* XMLDocumentationFileName() const;


 private:
  friend struct ToolConfiguration;
  std::unordered_map<std::string, std::string> properties;
};
