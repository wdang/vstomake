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
#include <string>

// simple string tokenizing 
//
// usage pattern:
// std::string some_string;
// StringTokenizer tokenizer(string," ");
// while(tokenizer.next()) {
//   std::string token = tokenizer.token();
//   // do something with token ...
// }

struct StringTokenizer {
  typedef std::string type;
  
  // construct a StringTokenizer to tokenize the given str
  // using the default delimiters:
  // ' '	(0x20)	space (SPC)
  // '\t'	(0x09)	horizontal tab (TAB)
  // '\n'	(0x0a)	newline (LF)
  // '\v'	(0x0b)	vertical tab (VT)
  // '\f'	(0x0c)	feed (FF)
  // '\r'	(0x0d)	carriage return 
  inline explicit StringTokenizer(const type& str): source(str),
    delims(" \t\n\v\f\r"),
    previous(0),
    current(0),
    end(0) {
  }
  
  // construct a StringTokenizer to tokenize the given str using
  // a user defined string of delimiters
  inline StringTokenizer(const type& str, const type& delimiters)
    : source(str),
    delims(delimiters),
    previous(0),
    current(0),
    end(0) {
  }

  // advances the tokenizer and
  // returns true if there are any more tokens
  inline bool next() {
    return next(delims);
  }

  // advances the tokenizer using the specified delimiters and
  // returns true if there are any more tokens
  inline bool next(const type& delimiters) {
    current = source.find_first_not_of(delimiters, current);
    if (current != type::npos) {
      end = source.find_first_of(delimiters, current);
      if (end != type::npos) {
        previous = current;
        current  = end;
      }
      else {
        end      = source.length();
        previous = current;
        current  = end;
      }
      return true;
    }
    return false;
  }

  // returns the current token
  inline type token() const{
    return source.substr(previous, end - previous);
  }

  // return the next token without
  // advancing this tokenizer
  inline type peek() const {
    return peek(delims);
  }

  // return the next token without
  // advancing the tokenizer, using the
  // specified delimieters
  inline type peek(const type& delimiters) const {
    size_t current_ = source.find_first_not_of(delimiters, current);
    size_t end_     = source.find_first_of(delimiters, current_);
    if (end == type::npos) {
      end_ = source.length();
    }
    return source.substr(current_, end_ - current_);
  }

  // returns the number of tokens remaining in this tokenizer.
  // the number returnedis equivalent to the remaining number of times
  // a call to next() will return true
  inline size_t count() {
    size_t previous_ = previous;
    size_t current_  = current;
    size_t end_      = end;
    size_t num       = 0;
    while (next(delims)) ++num;
    previous = previous_;
    current  = current_;
    end      = end_;
    return num;
  }

  // returns the number of tokens remaining in this tokenizer.
  // the number returned is equivalent to the remaining number of times
  // that a call to next(delimiters) will return true
  inline size_t count(const type& delimiters) {
    size_t previous_ = previous;
    size_t current_  = current;
    size_t end_      = end;
    size_t num       = 0;
    while (next(delimiters)) ++num;
    previous = previous_;
    current  = current_;
    end      = end_;
    return num;
  }

  inline void reset() {
    previous = 0;
    current  = 0;
    end      = 0;
  }

 private:
  StringTokenizer(const StringTokenizer&);
  StringTokenizer& operator=(const StringTokenizer&);
  type       source;
  const type delims;
  size_t     previous, current, end;
};

#ifdef ENABLE_UNIT_TESTS
namespace {
 class StringTokenizerTest : public testing::Test {};

using std::string;
TEST(StringTokenizerTest, token) {
  string input = " The quick brown fox jumps over the lazy dog ";
  StringTokenizer t(input, " ");

  for (int i = 0; i<2; ++i) {
    int tokens = 9;
    EXPECT_EQ(tokens--, t.count());

    EXPECT_TRUE(t.next());
    EXPECT_EQ(string("The"), t.token());
    EXPECT_EQ(tokens--, t.count());

    EXPECT_TRUE(t.next());
    EXPECT_EQ(string("quick"), t.token());
    EXPECT_EQ(tokens--, t.count());

    EXPECT_TRUE(t.next());
    EXPECT_EQ(string("brown"), t.token());

    EXPECT_TRUE(t.next());
    EXPECT_EQ(string("fox"), t.token());

    EXPECT_TRUE(t.next());
    EXPECT_EQ(string("jumps"), t.token());

    EXPECT_TRUE(t.next());
    EXPECT_EQ(string("over"), t.token());

    EXPECT_TRUE(t.next());
    EXPECT_EQ(string("the"), t.token());


    EXPECT_TRUE(t.next());
    EXPECT_EQ(string("lazy"), t.token());

    EXPECT_TRUE(t.next());
    EXPECT_EQ(string("dog"), t.token());

    EXPECT_FALSE(t.next());
    t.reset();
  }
}
}
#endif