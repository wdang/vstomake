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
#include "vclinkertool.h"
namespace vs {
VCLinkerTool::Enum GetEnum(const std::string&){
  return VCLinkerTool::Enum_Unknown;
}

#define VCLINKERTOOL_ENUM_ACCESSORS X(AssemblyDebug)\
X(CLRImageType)\
X(CLRThreadAttribute)\
X(EnableCOMDATFolding)\
X(ErrorReporting)\
X(FixedBaseAddress)\
X(LargeAddressAware)\
X(LinkIncremental)\
X(LinkTimeCodeGeneration)\
X(OptimizeForWindows98)\
X(OptimizeReferences)\
X(ShowProgress)\
X(SubSystem)\
X(TargetMachine)\
X(TerminalServerAware)\
X(driver)

#define X(NAME) VCLinkerTool::Enum VCLinkerTool::NAME() const {\
  auto iter = properties->find(#NAME);\
  VCLinkerTool::Enum rv;\
  if(iter != properties->end()){\
     rv = GetEnum(iter->second);\
  }else{\
    rv = Enum_Unknown;\
  }\
  return rv;\
}
VCLINKERTOOL_ENUM_ACCESSORS
#undef X




#define VCLINKERTOOL_BOOL_ACCESSORS X(AllowIsolation)\
X(CLRUnmanagedCodeCheck)\
X(DelaySign)\
X(GenerateDebugInformation)\
X(GenerateManifest)\
X(GenerateMapFile)\
X(IgnoreAllDefaultLibraries)\
X(IgnoreEmbeddedIDL)\
X(IgnoreImportLibrary)\
X(LinkDLL)\
X(LinkLibraryDependencies)\
X(MapExports)\
X(Profile)\
X(RegisterOutput)\
X(ResourceOnlyDLL)\
X(SetChecksum)\
X(SupportUnloadOfDelayLoadedDLL)\
X(SuppressStartupBanner)\
X(SwapRunFromCD)\
X(SwapRunFromNet)\
X(TurnOffAssemblyGeneration)\
X(UseLibraryDependencyInputs)\
X(UseUnicodeResponseFiles)

#define X(NAME) bool VCLinkerTool::NAME() const {\
  auto iter = properties->find(#NAME);\
  bool rv = false;\
  if(iter != properties->end()){\
    std::string value(iter->second);\
    std::transform(value.begin(),value.end(),value.begin(),tolower);\
    rv = (value.find("true") != std::string::npos);\
  }\
  return rv;\
}
VCLINKERTOOL_BOOL_ACCESSORS
#undef X



#define VCLINKERTOOL_STRING_ACCESSORS X(AddModuleNamesToAssembly)\
X(AdditionalDependencies)\
X(AdditionalLibraryDirectories)\
X(AdditionalManifestDependencies)\
X(AdditionalOptions)\
X(AssemblyLinkResource)\
X(BaseAddress)\
X(DelayLoadDLLs)\
X(EmbedManagedResourceFile)\
X(EntryPointSymbol)\
X(ExecutionBucket)\
X(ForceSymbolReferences)\
X(FunctionOrder)\
X(HeapCommitSize)\
X(HeapReserveSize)\
X(IgnoreDefaultLibraryNames)\
X(ImportLibrary)\
X(KeyContainer)\
X(KeyFile)\
X(ManifestFile)\
X(MapFileName)\
X(MergeSections)\
X(MergedIDLBaseFileName)\
X(MidlCommandFile)\
X(ModuleDefinitionFile)\
X(OutputFile)\
X(ProfileGuidedDatabase)\
X(ProgramDatabaseFile)\
X(StackCommitSize)\
X(StackReserveSize)\
X(StripPrivateSymbols)\
X(ToolKind)\
X(ToolPath)\
X(TypeLibraryFile)\
X(TypeLibraryResourceID)\
X(VCProjectEngine)\
X(Version)\
X(toolName)

#define X(NAME) const char* VCLinkerTool::NAME() const { \
    auto iter = properties->find(# NAME); \
    return iter == properties->end() ? "" : iter->second.c_str(); \
}
VCLINKERTOOL_STRING_ACCESSORS
#undef X

}