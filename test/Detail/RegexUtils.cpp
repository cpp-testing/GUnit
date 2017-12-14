//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <gtest/gtest.h>

#include "GUnit/Detail/RegexUtils.h"
#include "GUnit/Detail/StringUtils.h"

namespace testing {
inline namespace v1 {
namespace detail {

TEST(RegexUtils, ShouldReturnMatches) {
  constexpr auto n0 = "I press add"_gtest_string;
  constexpr auto n1 = "I have a {number} to read"_gtest_string;
  constexpr auto n2 =
      "I have a {number} and a {second number} to read"_gtest_string;
  constexpr auto n3 =
      R"(I have a '{text}' and a {second number} to read)"_gtest_string;
  constexpr auto n4 = "I have a {} blah"_gtest_string;
  constexpr auto n5 = "I have a {} to read"_gtest_string;
  constexpr auto n6 = R"(I have a '{}' to read)"_gtest_string;
  constexpr auto n7 = "I have the following {table}"_gtest_string;
  constexpr auto n8 = "I access table with 1 row"_gtest_string;
  constexpr auto n9 = "I access table with {n}"_gtest_string;
  constexpr auto n10 = "I have a: {a}, b: {b}"_gtest_string;

  constexpr auto t0 = "I press add";
  constexpr auto t1 = "I have a 42 to read";
  constexpr auto t2 = "I have a 1234 and a fifty to read";
  constexpr auto t3 = R"(I have a 'text with spaces' and a fifty to read)";
  constexpr auto t4 = R"(I have a '42' to read)";
  constexpr auto t5 = "I have the following table";
  constexpr auto t6 = "I access table with 1 row";
  constexpr auto t7 = "I access table with 42";
  constexpr auto t8 = "I have a: 1, b: 2";
  constexpr auto t9 = "I have a: value";
  constexpr auto t10 = "I have a: {v}";
  constexpr auto t11 = "I have a: {value}";

  static_assert(0 == args_size(n0), "");
  static_assert(1 == args_size(n1), "");
  static_assert(2 == args_size(n2), "");
  static_assert(2 == args_size(n3), "");
  static_assert(1 == args_size(n9), "");

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

  EXPECT_TRUE(match(n8, t6));
  EXPECT_FALSE(match(n8, t7));
  EXPECT_TRUE(match(n9, t7));
  EXPECT_FALSE(match(n9, t6));

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

  EXPECT_EQ(1u, matches(n7, t5).size());
  EXPECT_EQ("table", matches(n7, t5)[0]);

  EXPECT_EQ(2u, matches(n10, t8).size());
  EXPECT_EQ("1", matches(n10, t8)[0]);
  EXPECT_EQ("2", matches(n10, t8)[1]);

  EXPECT_EQ(0u, matches(n10, t9).size());
  EXPECT_EQ(0u, matches(n10, t10).size());
  EXPECT_EQ(0u, matches(n10, t11).size());
}

}  // detail
}  // v1
}  // testing
