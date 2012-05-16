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

#include <string>
#include <unordered_map>
#include <vector>

void GeneratePropertyMappings(std::unordered_map<std::string, std::vector<std::string> >* out);
void Test();


// String search and replace (in place)
//
// @source: pointer to a string used for the search
// @substring: string to search for in source 
// @replacement: string to replace substring with in source
// 
// Returns the number of substrings replaced
inline size_t Replace(std::string* source, 
       const std::string& substring, const std::string& replacement) {
  size_t pos = 0;
  while ((pos = source->find(substring, pos)) != std::string::npos) {
    source->replace(pos, substring.length(), replacement);
    pos += replacement.length();
  }
  return replacement.length() ? pos/replacement.length(): 0;
}

// String search and replace.
//
// @source: string used for the search
// @substring: the string to search for in source 
// @replacement: the string to replace substring with in source
// 
// Returns a copy of source with each substring replaced with the given 
// replacement string
inline std::string Replace(const std::string& source, 
       const std::string& substring, const std::string& replacement) {
  std::string rv(source);
  Replace(&rv,substring,replacement);
  return rv;
}


// Returns a copy of source with all backslashes 
// replaced with forward slashes
// does not verify that source is an actual path string
inline std::string ToUnixPaths(const std::string& source) {
  return Replace(source,"\\","/");
}

// Removes leading ./ and .\ from paths
inline std::string StripCurrentDirReference(const std::string& path) {
  if(path.size() && path[0] == '.') {
     if(path[1] == '/' || path[1] == '\\')
       return path.substr(2);
  }
  return path;
}