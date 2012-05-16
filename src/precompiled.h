#ifndef VSTOMAKE_PRECOMPILED_H
#define VSTOMAKE_PRECOMPILED_H
#ifdef _MSC_VER
#pragma once
#endif

// C
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// define standard integer types for vs2008 and below
#if !defined(_MSC_VER) || (_MSC_VER >=1600)
#include <stdint.h>
#else
typedef signed __int8    int8_t;
typedef signed __int16   int16_t;
typedef signed __int32   int32_t;
typedef signed __int64   int64_t;
typedef unsigned __int8  uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;

// 7.18.1.2 Minimum-width integer types
typedef signed __int8    int_least8_t;
typedef signed __int16   int_least16_t;
typedef signed __int32   int_least32_t;
typedef unsigned __int8  uint_least8_t;
typedef unsigned __int16 uint_least16_t;
typedef unsigned __int32 uint_least32_t;

// 7.18.1.3 Fastest minimum-width integer types
typedef signed __int8 int_fast8_t;
typedef signed int    int_fast16_t;
typedef signed int    int_fast32_t;
typedef int64_t       int_fast64_t;
typedef unsigned char uint_fast8_t;
typedef unsigned int  uint_fast16_t;
typedef unsigned int  uint_fast32_t;
typedef uint64_t      uint_fast64_t;

// 7.18.1.5 Greatest-width integer types
typedef int64_t  intmax_t;
typedef uint64_t uintmax_t;
#endif


// C++
#include <string>
#include <algorithm>
#include <functional>
#include <memory>
#include <utility>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <functional>

// External
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH




#ifdef VSTOMAKE_RUN_TESTS
#include "gtest/gtest.h"
#endif

#endif //VSTOMAKE_PRECOMPILED_H
// Copyright 2011 William Dang.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
