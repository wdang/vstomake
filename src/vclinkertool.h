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
#include "vcproject.h"

// VCLinkerTool is for convienent access
// to the VCLinkerTool  properties of a project configuration
struct VCLinkerTool {

  // Access VCLinkerTool properties from the given
  // project configuration
  explicit VCLinkerTool(VCProject::Configuration& configuration);
  
  const char* AdditionalDependencies() const;
  const char* AdditionalLibraryDirectories() const;
  const char* AdditionalManifestDependencies() const;
  const char* AdditionalOptions() const;
  const char* AddModuleNamesToAssembly() const;
  const char* AllowIsolation() const;
  const char* AssemblyDebug() const;
  const char* AssemblyLinkResource() const;
  const char* BaseAddress() const;
  const char* CLRImageType() const;
  const char* CLRThreadAttribute() const;
  const char* CLRUnmanagedCodeCheck() const;
  const char* DataExecutionPrevention() const;
  const char* DelayLoadDLLs() const;
  const char* DelaySign() const;
  const char* driver() const;
  const char* EmbedManagedResourceFile() const;
  const char* EnableCOMDATFolding() const;
  const char* EnableUAC() const;
  const char* EntryPointSymbol() const;
  const char* ErrorReporting() const;
  const char* ExecutionBucket() const;
  const char* FixedBaseAddress() const;
  const char* ForceSymbolReferences() const;
  const char* FunctionOrder() const;
  const char* GenerateDebugInformation() const;
  const char* GenerateManifest() const;
  const char* GenerateMapFile() const;
  const char* HeapCommitSize() const;
  const char* HeapReserveSize() const;
  const char* IgnoreAllDefaultLibraries() const;
  const char* IgnoreDefaultLibraryNames() const;
  const char* IgnoreEmbeddedIDL() const;
  const char* IgnoreImportLibrary() const;
  const char* ImportLibrary() const;
  const char* KeyContainer() const;
  const char* KeyFile() const;
  const char* LargeAddressAware() const;
  const char* LinkDLL() const;
  const char* LinkIncremental() const;
  const char* LinkLibraryDependencies() const;
  const char* LinkTimeCodeGeneration() const;
  const char* ManifestFile() const;
  const char* MapExports() const;
  const char* MapFileName() const;
  const char* MergedIDLBaseFileName() const;
  const char* MergeSections() const;
  const char* MidlCommandFile() const;
  const char* ModuleDefinitionFile() const;
  const char* OptimizeForWindows98() const;
  const char* OptimizeReferences() const;
  const char* OutputFile() const;
  const char* PerUserRedirection() const;
  const char* Profile() const;
  const char* ProfileGuidedDatabase() const;
  const char* ProgramDatabaseFile() const;
  const char* RandomizedBaseAddress() const;
  const char* RegisterOutput() const;
  const char* ResourceOnlyDLL() const;
  const char* SetChecksum() const;
  const char* ShowProgress() const;
  const char* StackCommitSize() const;
  const char* StackReserveSize() const;
  const char* StripPrivateSymbols() const;
  const char* SubSystem() const;
  const char* SupportUnloadOfDelayLoadedDLL() const;
  const char* SuppressStartupBanner() const;
  const char* SwapRunFromCD() const;
  const char* SwapRunFromNet() const;
  const char* TargetMachine() const;
  const char* TerminalServerAware() const;
  const char* ToolKind() const;
  const char* toolName() const;
  const char* ToolPath() const;
  const char* TurnOffAssemblyGeneration() const;
  const char* TypeLibraryFile() const;
  const char* TypeLibraryResourceID() const;
  const char* UACExecutionLevel() const;
  const char* UACUIAccess() const;
  const char* UseLibraryDependencyInputs() const;
  const char* UseUnicodeResponseFiles() const;
  const char* VCProjectEngine() const;
  const char* Version() const;
  
private:
  std::unordered_map<std::string,std::string> properties;
};
