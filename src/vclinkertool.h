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
// VCLinkerTool is for convienent access
// to the VCLinkerTool  properties of a project configuration
struct VCLinkerTool {
  enum Enum {
    Enum_Unknown,
    Enum_addrAwareDefault,
    Enum_addrAwareLarge,
    Enum_addrAwareNoLarge,
    Enum_CLRImageTypeDefault,
    Enum_CLRImageTypeIJW,
    Enum_CLRImageTypePure,
    Enum_CLRImageTypeSafe,
    Enum_CLRThreadAttributeMTA,
    Enum_CLRThreadAttributeNone,
    Enum_CLRThreadAttributeSTA,
    Enum_driver,
    Enum_driverDefault,
    Enum_driverUpOnly,
    Enum_driverWDM,
    Enum_LinkTimeCodeGenerationOptionDefault,
    Enum_LinkTimeCodeGenerationOptionInstrument,
    Enum_LinkTimeCodeGenerationOptionOptimize,
    Enum_LinkTimeCodeGenerationOptionUpdate,
    Enum_LinkTimeCodeGenerationOptionUse,
    Enum_linkAssemblyDebugDefault,
    Enum_linkAssemblyDebugDisable,
    Enum_linkAssemblyDebugFull,
    Enum_linkFixedBaseAddressDefault,
    Enum_linkFixedBaseAddressNo,
    Enum_linkFixedBaseAddressYes,
    Enum_linkIncrementalDefault,
    Enum_linkIncrementalNo,
    Enum_linkIncrementalYes,
    Enum_linkProgressAll,
    Enum_linkProgressLibs,
    Enum_linkProgressNotSet,
    Enum_linkerErrorReportingDefault,
    Enum_linkerErrorReportingPrompt,
    Enum_linkerErrorReportingQueue,
    Enum_machineAM33,
    Enum_machineAMD64,
    Enum_machineARM,
    Enum_machineEBC,
    Enum_machineIA64,
    Enum_machineM32R,
    Enum_machineMIPS,
    Enum_machineMIPS16,
    Enum_machineMIPSFPU,
    Enum_machineMIPSFPU16,
    Enum_machineMIPSR41XX,
    Enum_machineNotSet,
    Enum_machineSH3,
    Enum_machineSH3DSP,
    Enum_machineSH4,
    Enum_machineSH5,
    Enum_machineTHUMB,
    Enum_machineX86,
    Enum_optFolding,
    Enum_optFoldingDefault,
    Enum_optNoFolding,
    Enum_optNoReferences,
    Enum_optReferences,
    Enum_optReferencesDefault,
    Enum_optWin98Default,
    Enum_optWin98No,
    Enum_optWin98Yes,
    Enum_subSystemConsole,
    Enum_subSystemEFIApplication,
    Enum_subSystemEFIBootService,
    Enum_subSystemEFIROM,
    Enum_subSystemEFIRuntime,
    Enum_subSystemNative,
    Enum_subSystemNotSet,
    Enum_subSystemPosix,
    Enum_subSystemWindows,
    Enum_subSystemWindowsCE,
    Enum_termSvrAwareDefault,
    Enum_termSvrAwareNo,
    Enum_termSvrAwareYes,
  };

  // Access VCLinkerTool properties from the given
  // project configuration

  // The following accessors return "" or Enum_Unknown or false 
  // to represent an empty value
  const char* AdditionalDependencies() const;
  const char* AdditionalLibraryDirectories() const;
  const char* AdditionalManifestDependencies() const;
  const char* AdditionalOptions() const;
  const char* AddModuleNamesToAssembly() const;
  bool        AllowIsolation() const;
  Enum        AssemblyDebug() const;
  const char* AssemblyLinkResource() const;
  const char* BaseAddress() const;
  Enum        CLRImageType() const;
  Enum        CLRThreadAttribute() const;
  bool        CLRUnmanagedCodeCheck() const;
  const char* DelayLoadDLLs() const;
  bool        DelaySign() const;
  Enum        driver() const;
  const char* EmbedManagedResourceFile() const;
  Enum        EnableCOMDATFolding() const;
  const char* EntryPointSymbol() const;
  Enum        ErrorReporting() const;
  const char* ExecutionBucket() const;
  Enum        FixedBaseAddress() const;
  const char* ForceSymbolReferences() const;
  const char* FunctionOrder() const;
  bool        GenerateDebugInformation() const;
  bool        GenerateManifest() const;
  bool        GenerateMapFile() const;
  const char* HeapCommitSize() const;
  const char* HeapReserveSize() const;
  bool        IgnoreAllDefaultLibraries() const;
  const char* IgnoreDefaultLibraryNames() const;
  bool        IgnoreEmbeddedIDL() const;
  bool        IgnoreImportLibrary() const;
  const char* ImportLibrary() const;
  const char* KeyContainer() const;
  const char* KeyFile() const;
  Enum        LargeAddressAware() const;
  bool        LinkDLL() const;
  Enum        LinkIncremental() const;
  bool        LinkLibraryDependencies() const;
  Enum        LinkTimeCodeGeneration() const;
  const char* ManifestFile() const;
  bool        MapExports() const;
  const char* MapFileName() const;
  const char* MergedIDLBaseFileName() const;
  const char* MergeSections() const;
  const char* MidlCommandFile() const;
  const char* ModuleDefinitionFile() const;
  Enum        OptimizeForWindows98() const;
  Enum        OptimizeReferences() const;
  const char* OutputFile() const;
  bool        Profile() const;
  const char* ProfileGuidedDatabase() const;
  const char* ProgramDatabaseFile() const;
  bool        RegisterOutput() const;
  bool        ResourceOnlyDLL() const;
  bool        SetChecksum() const;
  Enum        ShowProgress() const;
  const char* StackCommitSize() const;
  const char* StackReserveSize() const;
  const char* StripPrivateSymbols() const;
  Enum        SubSystem() const;
  bool        SupportUnloadOfDelayLoadedDLL() const;
  bool        SuppressStartupBanner() const;
  bool        SwapRunFromCD() const;
  bool        SwapRunFromNet() const;
  Enum        TargetMachine() const;
  Enum        TerminalServerAware() const;
  const char* ToolKind() const;
  const char* toolName() const;
  const char* ToolPath() const;
  bool        TurnOffAssemblyGeneration() const;
  const char* TypeLibraryFile() const;
  const char* TypeLibraryResourceID() const;
  bool        UseLibraryDependencyInputs() const;
  bool        UseUnicodeResponseFiles() const;
  const char* VCProjectEngine() const;
  const char* Version() const;


 private:
  friend struct VCProject;
  friend struct VCConfiguration;  
  std::unordered_map<std::string, std::string>* properties;
};
