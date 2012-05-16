#include "precompiled.h"
#include "vcprojectengine.h"
#include <rapidxml/rapidxml.hpp>
#include "vcclcompilertool.h"
#include "vclinkertool.h"

using std::string;
using std::istringstream;
using std::ifstream;
using std::vector;
using std::unordered_map;
using namespace rapidxml;

#define VCPROJECT_ENGINE_ENUMERATIONS X(addressAwarenessType) \
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
static const char* kVCProjectEngineEnumStrings[] = {VCPROJECT_ENGINE_ENUMERATIONS};
#undef X
namespace internal {
void GetVCProjectEngineEnumList(std::unordered_map<std::string, std::vector<std::string> >* out) {
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

  xml_node<>* start  = doc.first_node("doc")->first_node("members")->first_node();
  auto& enumerations = *out;
  for (int i = 0, end =
         sizeof(kVCProjectEngineEnumStrings)/sizeof(kVCProjectEngineEnumStrings[0]); i<end; ++i) {

    const char* current_enum = kVCProjectEngineEnumStrings[i];
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
}

#define VCPROJ_TOOL_INTERFACES X(VCALinkTool) \
X(VCAppVerifierTool) \
X(VCBscMakeTool) \
X(VCCLCompilerTool) \
X(VCCustomBuildTool) \
X(VCFxCopTool) \
X(VCLibrarianTool) \
X(VCLinkerTool) \
X(VCManagedResourceCompilerTool) \
X(VCManifestTool) \
X(VCMidlTool) \
X(VCNMakeTool) \
X(VCPostBuildEventTool) \
X(VCPreBuildEventTool) \
X(VCPreLinkEventTool) \
X(VCResourceCompilerTool) \
X(VCWebDeploymentTool) \
X(VCWebServiceProxyGeneratorTool) \
X(VCXDCMakeTool) \
X(VCXMLDataGeneratorTool)




// tool interface strings
#define X(NAME) # NAME,
static const char* kToolStrings[] = {VCPROJ_TOOL_INTERFACES};
#undef X

#define X(NAME) void Init##NAME##Properties(std::unordered_map<std::string,std::string>*);
VCPROJ_TOOL_INTERFACES;
#undef X

#define X(NAME) Init##NAME##Properties,
typedef void(*ToolInitFunction)(std::unordered_map<std::string,std::string>*);
static ToolInitFunction kToolFunctions[] = {VCPROJ_TOOL_INTERFACES};
#undef X

void InitToolProperties( const std::string& toolname, std::unordered_map<std::string,std::string>* out ){
    for(size_t i = 0;i < sizeof(kToolStrings)/sizeof(kToolStrings[0]);++i){
      if (toolname.compare(kToolStrings[i]) == 0){
        kToolFunctions[i](out);
        break;
      }
    }
}






#define VCLIBRARIANTOOL_IMPL X(AdditionalDependencies)\
X(AdditionalLibraryDirectories)\
X(AdditionalOptions)\
X(ExecutionBucket)\
X(ExportNamedFunctions)\
X(ForceSymbolReferences)\
X(IgnoreAllDefaultLibraries)\
X(IgnoreDefaultLibraryNames)\
X(Inputs)\
X(LinkLibraryDependencies)\
X(LinkTimeCodeGeneration)\
X(ModuleDefinitionFile)\
X(OutputFile)\
X(SuppressStartupBanner)\
X(ToolKind)\
X(toolName)\
X(ToolPath)\
X(UseUnicodeResponseFiles)\
X(VCProjectEngine)

#define X(NAME) const char* VCLibrarianTool::NAME() const {\
    auto iter = properties.find(#NAME);\
    return iter->second.c_str();\
}
VCLIBRARIANTOOL_IMPL
#undef X

VCLibrarianTool::VCLibrarianTool( const std::unordered_map<std::string,std::string>& tool_properties )
:properties(tool_properties){
}




void InitVCALinkToolProperties(unordered_map<string,string>*){}
void InitVCAppVerifierToolProperties(unordered_map<string,string>*){}
void InitVCBscMakeToolProperties(unordered_map<string,string>*){}
//void InitVCCLCompilerToolProperties(unordered_map<string,string>*){}
void InitVCCustomBuildToolProperties(unordered_map<string,string>*){}
void InitVCFxCopToolProperties(unordered_map<string,string>*){}
void InitVCLibrarianToolProperties(unordered_map<string,string>*){}
//void InitVCLinkerToolProperties(unordered_map<string,string>*){}
void InitVCManagedResourceCompilerToolProperties(unordered_map<string,string>*){}
void InitVCManifestToolProperties(unordered_map<string,string>*){}
void InitVCMidlToolProperties(unordered_map<string,string>*){}
void InitVCNMakeToolProperties(unordered_map<string,string>*){}
void InitVCPostBuildEventToolProperties(unordered_map<string,string>*){}
void InitVCPreBuildEventToolProperties(unordered_map<string,string>*){}
void InitVCPreLinkEventToolProperties(unordered_map<string,string>*){}
void InitVCResourceCompilerToolProperties(unordered_map<string,string>*){}
void InitVCWebDeploymentToolProperties(unordered_map<string,string>*){}
void InitVCWebServiceProxyGeneratorToolProperties(unordered_map<string,string>*){}
void InitVCXDCMakeToolProperties(unordered_map<string,string>*){}
void InitVCXMLDataGeneratorToolProperties(unordered_map<string,string>*){}
