//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/Detail/Utility.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

struct a {};

namespace testing {
inline namespace v1 {
namespace detail {

TEST(Utility, ShouldConvertStringToCharPtr) {
  EXPECT_STREQ("", string<>::c_str());
  EXPECT_STREQ("ab", (string<'a', 'b'>::c_str()));
}

TEST(Utility, ShouldAdd2Strings) {
  {
    auto str = string<>{} + string<>{};
    EXPECT_STREQ("", str.c_str());
  }

  {
    auto str = string<'a'>{} + string<'b'>{};
    EXPECT_STREQ("ab", str.c_str());
  }
}

TEST(Utility, ShouldMakeString) {
  using namespace operators;
  static_assert(std::is_same<string<>, decltype(""_gtest_string)>::value, "");
  static_assert(std::is_same<string<'a', 'b', 'c'>, decltype("abc"_gtest_string)>::value, "");

  struct String {
    const char* chrs = "abcd";
  };

  static_assert(std::is_same<string<'a', 'b', 'c', 'd', 0>, decltype(make_string<String, sizeof("abcd")>::type())>::value, "");
}

struct n {};

TEST(Utility, ShouldGetTypeName) {
  EXPECT_STREQ("int", get_type_name<int>());
  EXPECT_STREQ("const double", get_type_name<const double>());
  EXPECT_STREQ("a", get_type_name<a>());
  EXPECT_STREQ("testing::v1::detail::n", get_type_name<n>());
}

TEST(Utility, ShouldReturnBaseName) {
  EXPECT_EQ(std::string{"file.hpp"}, basename("file.hpp"));
  EXPECT_EQ(std::string{"file.hpp"}, basename("/a/b/file.hpp"));
  EXPECT_EQ(std::string{"file.hpp"}, basename("/b/file.hpp"));
}

TEST(Utility, ShouldReturnTrimmedString) {
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

TEST(Utility, ShouldReturnDemangledName) {
  EXPECT_EQ(std::string{}, demangle(""));
  EXPECT_EQ(std::string{}, demangle("!!!"));
  EXPECT_EQ(std::string{"std::ignore"}, demangle("_ZStL6ignore"));
  EXPECT_EQ(std::string{"std::vector<double, std::allocator<double> >::~vector()"}, demangle("_ZNSt6vectorIdSaIdEED1Ev"));
}

TEST(Utility, ShouldReturnProgFullPath) { EXPECT_EQ(internal::GetArgvs()[0], progname()); }

TEST(Utility, ShouldReturnCallStack) {
  EXPECT_EQ(std::string{}, call_stack("\n", 0));
  EXPECT_EQ(std::string{}, call_stack("\n", 1));
  EXPECT_THAT(call_stack("\n", 2), testing::MatchesRegex(".*Utility_ShouldReturnCallStack_Test.*"));
}

TEST(Utility, ShouldReturnSymbols) {
  struct Parser {
    using type = std::string;
    static auto parse(const std::string& line) { return line; }
  };
  const auto& parsed = symbols<Parser>("_ZTIN7testing2v16detail32Utility_ShouldReturnSymbols_TestE");
  ASSERT_TRUE(parsed.size() >= 1);
}

TEST(Utility, ShouldReturnFileAndLine) {
  const auto al = addr2line(__builtin_return_address(0));
#if defined(__APPLE__)  // addr2line is not installed by default
  EXPECT_EQ(std::string{}, al.first);
  EXPECT_TRUE(0 == al.second);
#else
#if defined(NDEBUG)
  EXPECT_TRUE(al.first == "??" || al.first == "");
#else
  EXPECT_THAT(al.first, testing::MatchesRegex(".*gtest.*"));
  EXPECT_TRUE(al.second > 0);
#endif
#endif
}
}
}
}
