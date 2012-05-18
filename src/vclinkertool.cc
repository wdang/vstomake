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

#define VCLINKERTOOL_IMPL X(AdditionalDependencies) \
  X(AdditionalLibraryDirectories) \
  X(AdditionalManifestDependencies) \
  X(AdditionalOptions) \
  X(AddModuleNamesToAssembly) \
  X(AllowIsolation) \
  X(AssemblyDebug) \
  X(AssemblyLinkResource) \
  X(BaseAddress) \
  X(CLRImageType) \
  X(CLRThreadAttribute) \
  X(CLRUnmanagedCodeCheck) \
  X(DataExecutionPrevention) \
  X(DelayLoadDLLs) \
  X(DelaySign) \
  X(driver) \
  X(EmbedManagedResourceFile) \
  X(EnableCOMDATFolding) \
  X(EnableUAC) \
  X(EntryPointSymbol) \
  X(ErrorReporting) \
  X(ExecutionBucket) \
  X(FixedBaseAddress) \
  X(ForceSymbolReferences) \
  X(FunctionOrder) \
  X(GenerateDebugInformation) \
  X(GenerateManifest) \
  X(GenerateMapFile) \
  X(HeapCommitSize) \
  X(HeapReserveSize) \
  X(IgnoreAllDefaultLibraries) \
  X(IgnoreDefaultLibraryNames) \
  X(IgnoreEmbeddedIDL) \
  X(IgnoreImportLibrary) \
  X(ImportLibrary) \
  X(KeyContainer) \
  X(KeyFile) \
  X(LargeAddressAware) \
  X(LinkDLL) \
  X(LinkIncremental) \
  X(LinkLibraryDependencies) \
  X(LinkTimeCodeGeneration) \
  X(ManifestFile) \
  X(MapExports) \
  X(MapFileName) \
  X(MergedIDLBaseFileName) \
  X(MergeSections) \
  X(MidlCommandFile) \
  X(ModuleDefinitionFile) \
  X(OptimizeForWindows98) \
  X(OptimizeReferences) \
  X(OutputFile) \
  X(PerUserRedirection) \
  X(Profile) \
  X(ProfileGuidedDatabase) \
  X(ProgramDatabaseFile) \
  X(RandomizedBaseAddress) \
  X(RegisterOutput) \
  X(ResourceOnlyDLL) \
  X(SetChecksum) \
  X(ShowProgress) \
  X(StackCommitSize) \
  X(StackReserveSize) \
  X(StripPrivateSymbols) \
  X(SubSystem) \
  X(SupportUnloadOfDelayLoadedDLL) \
  X(SuppressStartupBanner) \
  X(SwapRunFromCD) \
  X(SwapRunFromNet) \
  X(TargetMachine) \
  X(TerminalServerAware) \
  X(ToolKind) \
  X(toolName) \
  X(ToolPath) \
  X(TurnOffAssemblyGeneration) \
  X(TypeLibraryFile) \
  X(TypeLibraryResourceID) \
  X(UACExecutionLevel) \
  X(UACUIAccess) \
  X(UseLibraryDependencyInputs) \
  X(UseUnicodeResponseFiles) \
  X(VCProjectEngine) \
  X(Version)

#define X(NAME) const char* VCLinkerTool::NAME() const { \
    auto iter = properties.find(# NAME); \
    return iter == properties.end() ? "" : iter->second.c_str(); \
}
VCLINKERTOOL_IMPL
#undef X

VCLinkerTool::VCLinkerTool(VCProject::Configuration& configuration) {
  auto iter = configuration.properties.find("VCLinkerTool");
  if (iter != configuration.properties.end()) {
    properties = iter->second;
  }
}
