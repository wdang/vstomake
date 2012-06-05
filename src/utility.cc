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
#include <stdlib.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif 

#ifndef STRICT
#define STRICT 1
#endif
 
#ifndef NOMINMAX
#define NOMINMAX 1
#endif

#include <windows.h>
#endif

std::string AbsoluteFilePath( const std::string& path ){
  static const size_t kMaxPath = 32767;
  char buffer[kMaxPath];
  char cwd[kMaxPath];  
#ifdef _WIN32
  GetCurrentDirectoryA(kMaxPath,cwd);
  unsigned results = GetFullPathNameA(path.c_str(),kMaxPath,buffer,NULL);
  SetCurrentDirectoryA(cwd);
  if(!results){
    return path;
  }
#else
  realpath(path.c_str(),buffer);
#endif
  return std::string(buffer);
}