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
#include "utility.h"
#include <deque>
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <utility>

#define VCPROJECT_ENGINE_ENUMS X(addressAwarenessType) \
  X(AppVrfBaseLayerOptions) \
  X(asmListingOption) \
  X(AVPageHeapProtectionDirection) \
  X(basicRuntimeCheckOption) \
  X(browseInfoOption) \
  X(BuildWithPropertySheetType) \
  X(callingConventionOption) \
  X(charSet) \
  X(compileAsManagedOptions) \
  X(CompileAsOptions) \
  X(compilerErrorReportingType) \
  X(ConfigurationTypes) \
  X(cppExceptionHandling) \
  X(debugOption) \
  X(driverOption) \
  X(eAppProtectionOption) \
  X(eCLRImageType) \
  X(eCLRThreadAttribute) \
  X(eDebuggerTypes) \
  X(eFileType) \
  X(enhancedInstructionSetType) \
  X(enumFileFormat) \
  X(enumMPIAcceptModes) \
  X(enumResourceLangID) \
  X(enumSccEvent) \
  X(eSqlClrPermissionLevel) \
  X(eWebRefUrlBehavior) \
  X(favorSizeOrSpeedOption) \
  X(floatingPointModel) \
  X(genProxyLanguage) \
  X(inlineExpansionOption) \
  X(linkAssemblyDebug) \
  X(linkerErrorReportingType) \
  X(linkFixedBaseAddress) \
  X(linkIncrementalType) \
  X(linkProgressOption) \
  X(LinkTimeCodeGenerationOption) \
  X(machineTypeOption) \
  X(midlCharOption) \
  X(midlErrorCheckOption) \
  X(midlStructMemberAlignOption) \
  X(midlTargetEnvironment) \
  X(midlWarningLevelOption) \
  X(optFoldingType) \
  X(optimizeOption) \
  X(optRefType) \
  X(optWin98Type) \
  X(pchOption) \
  X(preprocessOption) \
  X(RemoteDebuggerType) \
  X(runtimeLibraryOption) \
  X(structMemberAlignOption) \
  X(subSystemOption) \
  X(termSvrAwarenessType) \
  X(TypeOfDebugger) \
  X(useOfATL) \
  X(useOfMfc) \
  X(warningLevelOption) \
  X(WholeProgramOptimizationTypes)

#define X(NAME) # NAME,
static const char* kProjectEngineEnumStrings[] = {VCPROJECT_ENGINE_ENUMS};
#undef X

#define VCPROJECT_ENGINE_PROPERTY_TYPES X(ApplicationProtection, eAppProtectionOption) \
  X(AssemblerOutput, asmListingOption) \
  X(AssemblyDebug, linkAssemblyDebug) \
  X(BasicRuntimeChecks, basicRuntimeCheckOption) \
  X(BrowseInformation, browseInfoOption) \
  X(CLRImageType, eCLRImageType) \
  X(CLRThreadAttribute, eCLRThreadAttribute) \
  X(CallingConvention, callingConventionOption) \
  X(CharacterSet, charSet) \
  X(CompileAs, CompileAsOptions) \
  X(CompileAsManaged, compileAsManagedOptions) \
  X(ConfigurationType, ConfigurationTypes) \
  X(Culture, enumResourceLangID) \
  X(DebugInformationFormat, debugOption) \
  X(DebuggerFlavor, eDebuggerTypes) \
  X(DebuggerType, TypeOfDebugger) \
  X(DefaultCharType, midlCharOption) \
  X(EnableCOMDATFolding, optFoldingType) \
  X(EnableEnhancedInstructionSet, enhancedInstructionSetType) \
  X(EnableErrorChecks, midlErrorCheckOption) \
  X(ErrorReporting, compilerErrorReportingType) \
  X(ErrorReporting, linkerErrorReportingType) \
  X(ExceptionHandling, cppExceptionHandling) \
  X(FavorSizeOrSpeed, favorSizeOrSpeedOption) \
  X(FileType, eFileType) \
  X(FixedBaseAddress, linkFixedBaseAddress) \
  X(GeneratePreprocessedFile, preprocessOption) \
  X(GeneratedProxyLanguage, genProxyLanguage) \
  X(InlineFunctionExpansion, inlineExpansionOption) \
  X(LargeAddressAware, addressAwarenessType) \
  X(LinkIncremental, linkIncrementalType) \
  X(LinkTimeCodeGeneration, LinkTimeCodeGenerationOption) \
  X(MPIAcceptMode, enumMPIAcceptModes) \
  X(Optimization, optimizeOption) \
  X(OptimizeForWindows98, optWin98Type) \
  X(OptimizeReferences, optRefType) \
  X(Remote, RemoteDebuggerType) \
  X(RuntimeLibrary, runtimeLibraryOption) \
  X(ShowProgress, linkProgressOption) \
  X(SqlPermissionLevel, eSqlClrPermissionLevel) \
  X(StructMemberAlignment, midlStructMemberAlignOption) \
  X(StructMemberAlignment, structMemberAlignOption) \
  X(SubSystem, subSystemOption) \
  X(TargetEnvironment, midlTargetEnvironment) \
  X(TargetMachine, machineTypeOption) \
  X(TerminalServerAware, termSvrAwarenessType) \
  X(UrlBehavior, eWebRefUrlBehavior) \
  X(UsePrecompiledHeader, pchOption) \
  X(WarningLevel, midlWarningLevelOption) \
  X(WarningLevel, warningLevelOption) \
  X(WholeProgramOptimization, WholeProgramOptimizationTypes) \
  X(driver, driverOption) \
  X(floatingPointModel, floatingPointModel) \
  X(useOfATL, useOfATL) \
  X(useOfMfc, useOfMfc)

// kEnumTypeNames - an array of VCProjectEngine property strings
// that are associated with an enumeration type.
// VCProjectEngine properties that aren't in this array
// use "true" or "false" as their property values
#define X(NAME, ENUM_TYPE) # NAME,
static const char* kPropertyNames[] = {VCPROJECT_ENGINE_PROPERTY_TYPES};
#undef X

// kEnumTypeNames - an array of all enumeration strings
#define X(NAME, ENUM_TYPE) # ENUM_TYPE,
static const char* kEnumTypeNames[] = {VCPROJECT_ENGINE_PROPERTY_TYPES};
#undef X


void PrintXMacrosForVCCLCompilerProperties() {
  using std::string;
  using std::istringstream;
  using std::ifstream;
  using std::vector;
  using std::unordered_map;
  using namespace rapidxml;


  ifstream fsschema("testing\\Microsoft.VisualStudio2008.VCProjectEngine.xml",
                    std::ios::in | std::ios::binary | std::ios::ate);
  const int64_t size = fsschema.tellg();
  string schema(static_cast<unsigned>(size), '\0');

  if (fsschema.is_open()) {
  fsschema.seekg(0, std::ios::beg);
  fsschema.read(&schema[0], schema.size());
  fsschema.close();
  }

  xml_document<> doc;
  doc.parse<0>(const_cast<char*>(schema.c_str()));

  unordered_map<string, vector<string> > enums;
  xml_node<>* start = doc.first_node("doc")->first_node("members")->first_node();
  while (start) {
    string buffer(1024, '\0');

    xml_attribute<>* attr = start->first_attribute("name");
    if (char* ptr = strstr(attr->value(), "P:Microsoft.VisualStudio.VCProjectEngine.VCCLCompilerTool.")) {
      buffer.assign(strrchr(attr->value(), '.')+1);
      buffer.append(",");
      if (xml_node<>* returns = start->first_node("returns")) {
        if (xml_node<>* see = returns->first_node("see")) {
          buffer.append(strrchr(see->first_attribute()->value(), '.')+1);
        }
        else {
          string returns_string(returns->value());
          std::transform(returns_string.begin(), returns_string.end(), returns_string.begin(), tolower);
          if (strstr(returns_string.c_str(), "boolean")  || strstr(returns_string.c_str(), "true") || strstr(returns_string.c_str(), "false")) {
            buffer.append("Boolean");
          }
          else {
            buffer.append("String");
          }
        }
      }
      else {
        buffer.append("<UNDETERMINATE>");
      }

      printf("X(%s)\\\n", buffer.c_str());
    }

    start = start->next_sibling();
  }
}

// Mapping of Enumerations to "Enumeration members" i.e
// runtimeLibraryOption ==> [rtMultiThreaded,rtMultiThreadedDebug, rtMultiThreadedDLL,rtMultiThreadedDebugDLL]
// see: http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.vcprojectengine.aspx

void MapEnumerations(const rapidxml::xml_document<>& doc,
    std::unordered_map<std::string, std::vector<std::string> >* enums) {
  using std::string;
  using std::istringstream;
  using std::ifstream;
  using std::unordered_map;
  using std::vector;
  using namespace rapidxml;

  xml_node<>* start = doc.first_node("doc")->first_node("members")->first_node();


  auto& enumerations = *enums;
  for (int i = 0, end =
         sizeof(kProjectEngineEnumStrings)/sizeof(kProjectEngineEnumStrings[0]); i<end; ++i) {

  const char* current_enum = kProjectEngineEnumStrings[i];
  xml_node<>* current      = start;
  while (current) {
    xml_attribute<>* attr = current->first_attribute("name");
    if (char* ptr = strstr(attr->value(), current_enum)) {
      if (char* enumeration  = strrchr(ptr, '.')) {
        enumerations[current_enum].push_back(enumeration  + 1);
      }
    }
    current = current->next_sibling();
  }
  }
}

// Map properties of VCProjectEngine interfaces => to their
// corresponsing "Enumeration members" i.e
// RuntimeLibrary ==> [rtMultiThreaded,rtMultiThreadedDebug, rtMultiThreadedDLL,rtMultiThreadedDebugDLL]
//
// Mappings are generated by parsing the Microsoft.VisualStudio.VCProjectEngine.xml
// file located in the PublicAssemblies folder of your visual studio installation.
//
// This project contains a copy of these files:
// Microsoft.VisualStudio2008.VCProjectEngine.xml - for msvc 2008
// Microsoft.VisualStudio2010.VCProjectEngine.xml - for msvc 2010
void GeneratePropertyMappings(std::unordered_map<std::string, std::vector<std::string> >* out) {
  using std::string;
  using std::istringstream;
  using std::ifstream;
  using std::vector;
  using std::unordered_map;
  using namespace rapidxml;


  ifstream fsschema("testing\\Microsoft.VisualStudio2008.VCProjectEngine.xml",
                    std::ios::in | std::ios::binary | std::ios::ate);
  const int64_t size = fsschema.tellg();
  string schema(static_cast<unsigned>(size), '\0');

  if (fsschema.is_open()) {
  fsschema.seekg(0, std::ios::beg);
  fsschema.read(&schema[0], schema.size());
  fsschema.close();
  }

  xml_document<> doc;
  doc.parse<0>(const_cast<char*>(schema.c_str()));

  unordered_map<string, vector<string> > enums;
  MapEnumerations(doc, &enums);

  for (int i = 0, len = sizeof(kPropertyNames)/sizeof(kPropertyNames[0]); i<len; ++i) {
    string property(kPropertyNames[i]);
    string enum_type(kEnumTypeNames[i]);
    auto iter = enums.find(enum_type);
    if (iter != enums.end()) {
      out->insert(make_pair(property, iter->second));
    }
  }
}


#define VCPROJECT_ENGINE_PROPERTIES X(addressAwarenessType) \
  X(AppVrfBaseLayerOptions) \
  X(asmListingOption) \
  X(AVPageHeapProtectionDirection) \
  X(basicRuntimeCheckOption) \
  X(browseInfoOption) \
  X(BuildWithPropertySheetType) \
  X(callingConventionOption) \
  X(charSet) \
  X(compileAsManagedOptions) \
  X(CompileAsOptions) \
  X(compilerErrorReportingType) \
  X(ConfigurationTypes) \
  X(cppExceptionHandling) \
  X(debugOption) \
  X(driverOption) \
  X(eAppProtectionOption) \
  X(eCLRImageType) \
  X(eCLRThreadAttribute) \
  X(eDebuggerTypes) \
  X(eFileType) \
  X(enhancedInstructionSetType) \
  X(enumFileFormat) \
  X(enumMPIAcceptModes) \
  X(enumResourceLangID) \
  X(enumSccEvent) \
  X(eSqlClrPermissionLevel) \
  X(eWebRefUrlBehavior) \
  X(favorSizeOrSpeedOption) \
  X(floatingPointModel) \
  X(genProxyLanguage) \
  X(inlineExpansionOption) \
  X(linkAssemblyDebug) \
  X(linkerErrorReportingType) \
  X(linkFixedBaseAddress) \
  X(linkIncrementalType) \
  X(linkProgressOption) \
  X(LinkTimeCodeGenerationOption) \
  X(machineTypeOption) \
  X(midlCharOption) \
  X(midlErrorCheckOption) \
  X(midlStructMemberAlignOption) \
  X(midlTargetEnvironment) \
  X(midlWarningLevelOption) \
  X(optFoldingType) \
  X(optimizeOption) \
  X(optRefType) \
  X(optWin98Type) \
  X(pchOption) \
  X(preprocessOption) \
  X(RemoteDebuggerType) \
  X(runtimeLibraryOption) \
  X(structMemberAlignOption) \
  X(subSystemOption) \
  X(termSvrAwarenessType) \
  X(TypeOfDebugger) \
  X(useOfATL) \
  X(useOfMfc) \
  X(warningLevelOption) \
  X(WholeProgramOptimizationTypes) \
  X(String) \
  X(Boolean) \
  X(None)

#define CLCOMPILER_PROPERTIES X(AdditionalIncludeDirectories, String) \
  X(AdditionalOptions, String) \
  X(AdditionalUsingDirectories, String) \
  X(AssemblerListingLocation, String) \
  X(AssemblerOutput, asmListingOption) \
  X(BasicRuntimeChecks, basicRuntimeCheckOption) \
  X(BrowseInformation, browseInfoOption) \
  X(BrowseInformationFile, String) \
  X(BufferSecurityCheck, Boolean) \
  X(CallingConvention, String) \
  X(CompileAs, String) \
  X(CompileAsManaged, compileAsManagedOptions) \
  X(CompileOnly, Boolean) \
  X(DebugInformationFormat, debugOption) \
  X(DefaultCharIsUnsigned, Boolean) \
  X(Detect64BitPortabilityProblems, Boolean) \
  X(DisableLanguageExtensions, Boolean) \
  X(DisableSpecificWarnings, String) \
  X(EnableEnhancedInstructionSet, enhancedInstructionSetType) \
  X(EnableFiberSafeOptimizations, Boolean) \
  X(EnableFunctionLevelLinking, Boolean) \
  X(EnableIntrinsicFunctions, Boolean) \
  X(EnablePREfast, Boolean) \
  X(ErrorReporting, String) \
  X(ExceptionHandling, cppExceptionHandling) \
  X(ExecutionBucket, String) \
  X(ExpandAttributedSource, Boolean) \
  X(FavorSizeOrSpeed, String) \
  X(FloatingPointExceptions, Boolean) \
  X(floatingPointModel, String) \
  X(ForceConformanceInForLoopScope, Boolean) \
  X(ForcedIncludeFiles, String) \
  X(ForcedUsingFiles, String) \
  X(FullIncludePath, String) \
  X(GeneratePreprocessedFile, preprocessOption) \
  X(GenerateXMLDocumentationFiles, Boolean) \
  X(IgnoreStandardIncludePath, Boolean) \
  X(InlineFunctionExpansion, String) \
  X(KeepComments, Boolean) \
  X(MinimalRebuild, Boolean) \
  X(ObjectFile, String) \
  X(OmitDefaultLibName, String) \
  X(OmitFramePointers, Boolean) \
  X(OpenMP, String) \
  X(Optimization, optimizeOption) \
  X(PrecompiledHeaderFile, String) \
  X(PrecompiledHeaderThrough, String) \
  X(PreprocessorDefinitions, String) \
  X(ProgramDataBaseFileName, String) \
  X(RuntimeLibrary, runtimeLibraryOption) \
  X(RuntimeTypeInfo, Boolean) \
  X(ShowIncludes, Boolean) \
  X(SmallerTypeCheck, Boolean) \
  X(StringPooling, Boolean) \
  X(StructMemberAlignment, structMemberAlignOption) \
  X(SuppressStartupBanner, Boolean) \
  X(ToolKind, String) \
  X(toolName, String) \
  X(ToolPath, String) \
  X(TreatWChar_tAsBuiltInType, Boolean) \
  X(UndefineAllPreprocessorDefinitions, Boolean) \
  X(UndefinePreprocessorDefinitions, String) \
  X(UseFullPaths, String) \
  X(UsePrecompiledHeader, pchOption) \
  X(UseUnicodeResponseFiles, Boolean) \
  X(VCProjectEngine, String) \
  X(WarnAsError, Boolean) \
  X(WarningLevel, String) \
  X(WholeProgramOptimization, Boolean) \
  X(XMLDocumentationFileName, String) \
  X(Unknown, None)


#define X(NAME) Type_ ## NAME,
enum Type { VCPROJECT_ENGINE_PROPERTIES};
#undef X

struct Property {
  const char* name;
  Type        type;
};

#define X(NAME, TYPE) { # NAME, Type_ ## TYPE },
Property kProperties[] = { CLCOMPILER_PROPERTIES };
#undef X

#define X(NAME, TYPE) # NAME,
static const char* kCLCompilerPropertyStrings[] = {CLCOMPILER_PROPERTIES};
#undef X


#define X(NAME, TYPE) sizeof(# NAME)-1,
static const size_t kCLCompilerPropertyLengths[] = {CLCOMPILER_PROPERTIES};
#undef X

const char* GetGCCCompilerFlag(const char* property, const char* value) {
  return 0;
}

Type GetType(const char* prop) {


  if (!prop)
    return Type_None;

  Type type = Type_None;
  for (int i = 0, len = sizeof(kProperties)/sizeof(kProperties[0]); i< len; ++i) {
    Property& property = kProperties[i];
    if (strcmp(property.name, prop) == 0) {
      type = property.type;
      break;
    }
  }

  return type;
}
