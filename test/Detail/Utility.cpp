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

TEST(Utility, ShouldReturnMatches) {
  constexpr auto n0 = "I press add"_step;
  constexpr auto n1 = "I have a {number} to read"_step;
  constexpr auto n2 = "I have a {number} and a {second number} to read"_step;
  constexpr auto n3 = R"(I have a '{text}' and a {second number} to read)"_step;
  constexpr auto n4 = "I have a {} blah"_step;
  constexpr auto n5 = "I have a {} to read"_step;
  constexpr auto n6 = R"(I have a '{}' to read)"_step;

  constexpr auto t0 = "I press add";
  constexpr auto t1 = "I have a 42 to read";
  constexpr auto t2 = "I have a 1234 and a fifty to read";
  constexpr auto t3 = R"(I have a 'text with spaces' and a fifty to read)";
  constexpr auto t4 = R"(I have a '42' to read)";

  static_assert(0 == args_size(n0), "");
  static_assert(1 == args_size(n1), "");
  static_assert(2 == args_size(n2), "");
  static_assert(2 == args_size(n3), "");

  EXPECT_TRUE(match(n0, t0));
  EXPECT_TRUE(not match(n0, t1));
  EXPECT_TRUE(not match(n0, t2));
  EXPECT_TRUE(not match(n0, t3));

  EXPECT_TRUE(match(n1, t1));
  EXPECT_TRUE(not match(n1, t0));
  EXPECT_TRUE(not match(n1, t2));
  EXPECT_TRUE(not match(n1, t3));

  EXPECT_TRUE(match(n2, t2));
  EXPECT_TRUE(not match(n2, t0));
  EXPECT_TRUE(not match(n2, t1));
  EXPECT_TRUE(not match(n2, t3));

  EXPECT_TRUE(match(n3, t3));
  EXPECT_TRUE(not match(n3, t0));
  EXPECT_TRUE(not match(n3, t1));
  EXPECT_TRUE(not match(n3, t2));

  EXPECT_TRUE(not match(n4, t1));

  EXPECT_TRUE(match(n5, t1));

  EXPECT_TRUE(match(n6, t4));
  EXPECT_TRUE(match(n6, t4));
  EXPECT_TRUE(not match(n6, t1));

  EXPECT_EQ(0u, matches(n4, t1).size());

  EXPECT_EQ(1u, matches(n1, t1).size());
  EXPECT_EQ("42", matches(n1, t1)[0]);

  EXPECT_EQ(2u, matches(n2, t2).size());
  EXPECT_EQ("1234", matches(n2, t2)[0]);
  EXPECT_EQ("fifty", matches(n2, t2)[1]);

  EXPECT_EQ(2u, matches(n3, t3).size());
  EXPECT_EQ("text with spaces", matches(n3, t3)[0]);
  EXPECT_EQ("fifty", matches(n3, t3)[1]);

  EXPECT_EQ(1u, matches(n5, t1).size());
  EXPECT_EQ("42", matches(n5, t1)[0]);

  EXPECT_EQ(1u, matches(n6, t4).size());
  EXPECT_EQ("42", matches(n6, t4)[0]);
}

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

TEST(Utility, ShouldReturnTrueIfIsValid) {
  auto has_f = is_valid([](auto&& x) -> decltype(x.f()) {});
  struct a {
    void f() {}
  };
  struct b {};
  EXPECT_TRUE(has_f(a{}));
  EXPECT_FALSE(has_f(b{}));
}

TEST(Utility, ShouldCall) {
  struct a {
    double foo() { return 77.0; };
  };

  EXPECT_EQ(77.0, (constexpr_if(is_valid([](auto&& x) -> decltype(x.foo()) {}), [](auto&& x) { return x.foo(); },
                                [](auto&&) { return 42; })(a{})));

  struct b {};

  EXPECT_EQ(42, (constexpr_if(is_valid([](auto&& x) -> decltype(x.foo()) {}), [](auto&& x) { return x.foo(); },
                              [](auto&&) { return 42; })(b{})));
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

#if !defined(__APPLE__)
TEST(Utility, ShouldReturnProgFullPath) { EXPECT_EQ(internal::GetArgvs()[0], progname()); }
#endif

TEST(Utility, ShouldReturnCallStack) {
  EXPECT_EQ(std::string{}, call_stack("\n", 0, 0));
  EXPECT_EQ(std::string{}, call_stack("\n", 1, 0));
  EXPECT_THAT(call_stack("\n", 1, 1), testing::MatchesRegex(".*Utility_ShouldReturnCallStack_Test.*"));
  EXPECT_THAT(call_stack("\n", 1, 2), testing::MatchesRegex(".*Utility_ShouldReturnCallStack_Test.*"));
}
}
}
}
