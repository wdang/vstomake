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
#include "vcproject.h"

struct VCLibrarianTool  {
  
  explicit VCLibrarianTool(VCProject::Configuration& configuration);

  const char* AdditionalDependencies() const;
  const char* AdditionalLibraryDirectories() const;
  const char* AdditionalOptions() const;
  const char* ExecutionBucket() const;
  const char* ExportNamedFunctions() const;
  const char* ForceSymbolReferences() const;
  const char* IgnoreAllDefaultLibraries() const;
  const char* IgnoreDefaultLibraryNames() const;
  const char* Inputs() const;
  bool        LinkLibraryDependencies() const;
  bool        LinkTimeCodeGeneration() const;
  const char* ModuleDefinitionFile() const;
  const char* OutputFile() const;
  const char* SuppressStartupBanner() const;
  const char* ToolKind() const;
  const char* toolName() const;
  const char* ToolPath() const;
  const char* UseUnicodeResponseFiles() const;
  const char* VCProjectEngine() const;
 
 private:
  std::unordered_map<std::string, std::string> properties;
};