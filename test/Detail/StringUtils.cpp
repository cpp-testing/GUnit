//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <gtest/gtest.h>

#include "GUnit/Detail/StringUtils.h"

namespace testing {
inline namespace v1 {
namespace detail {

TEST(StringUtils, ShouldConvertStringToCharPtr) {
  EXPECT_STREQ("", string<>::c_str());
  EXPECT_STREQ("ab", (string<'a', 'b'>::c_str()));
}

TEST(StringUtils, ShouldAdd2Strings) {
  {
    auto str = string<>{} + string<>{};
    EXPECT_STREQ("", str.c_str());
  }

  {
    auto str = string<'a'>{} + string<'b'>{};
    EXPECT_STREQ("ab", str.c_str());
  }
}

TEST(StringUtils, ShouldMakeString) {
  static_assert(std::is_same<string<>, decltype(""_gtest_string)>::value, "");
  static_assert(
      std::is_same<string<'a', 'b', 'c'>, decltype("abc"_gtest_string)>::value,
      "");

  struct String {
    const char* chrs = "abcd";
  };

  static_assert(
      std::is_same<string<'a', 'b', 'c', 'd', 0>,
                   decltype(
                       make_string<String, sizeof("abcd")>::type())>::value,
      "");
}

TEST(StringUtils, ShouldReturnTrimmedString) {
  {
    std::string str = "";
    trim(str);
    EXPECT_EQ(std::string{}, str);
  }
  {
    std::string str = "abc";
    trim(str);
    EXPECT_EQ(std::string{"abc"}, str);
  }
  {
    std::string str = "   abc ";
    trim(str);
    EXPECT_EQ(std::string{"abc"}, str);
  }

  {
    std::string str = "abc      ";
    trim(str);
    EXPECT_EQ(std::string{"abc"}, str);
  }
}

TEST(StringUtils, ShouldConvertToType) {
  EXPECT_EQ("str", lexical_cast<std::string>("str"));

  EXPECT_EQ(42, lexical_cast<int>("42"));
  EXPECT_EQ(99u, lexical_cast<std::size_t>("99"));
  EXPECT_EQ(0x30, lexical_cast<int>("0x30"));

  EXPECT_EQ(true, lexical_cast<bool>("true"));
  EXPECT_EQ(true, lexical_cast<bool>("1"));
}

}  // detail
}  // v1
}  // testing
