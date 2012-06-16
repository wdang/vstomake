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


// volatile internal header
#include <string>
#include <unordered_map>

#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_print.hpp>

namespace vs {

void FilterCPPSources(const std::vector<struct File*>& sources, std::vector<struct File*>* out);

template<class Generator>
extern void GenerateMakefile(const Generator& gen);

// Performs an in-place expansion 
// of project build macros for the given input string 
// using the given map of macro definitions.
// 
// @input: target for macro expansion
// @macros: definitions used for macro expansion
// 
// keys:
// http://msdn.microsoft.com/en-us/library/c02as0cs(VS.90).aspx
void ExpandMacros(std::string* input, 
   std::unordered_map<std::string,std::string>& macros);

// Returns a copy of the result of expanding the 
// given input string using the given ma of macro definitions
//
// @input: target for macro expansion
// @macros: definitions used for macro expansion
//
// Returns a copy of the result
std::string ExpandMacros(const std::string& input, 
   std::unordered_map<std::string,std::string>& macros);


bool GetToolPropertiesForConfiguration(const std::string& name, const std::string& toolname,
 rapidxml::xml_node<>* configurations, std::unordered_map<std::string, std::string>* props);
 

}








