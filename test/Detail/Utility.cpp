//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <gtest/gtest.h>

#include "GUnit/Detail/Utility.h"

namespace testing {
inline namespace v1 {
namespace detail {

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

  EXPECT_EQ(77.0,
            (constexpr_if(is_valid([](auto&& x) -> decltype(void(x.foo())) {}),
                          [](auto&& x) { return x.foo(); },
                          [](auto&&) { return 42; })(a{})));

  struct b {};

  EXPECT_EQ(42,
            (constexpr_if(is_valid([](auto&& x) -> decltype(void(x.foo())) {}),
                          [](auto&& x) { return x.foo(); },
                          [](auto&&) { return 42; })(b{})));
}

TEST(Utility, ShouldReturnTrueIfTupleContaintsType) {
  EXPECT_FALSE((contains<int, std::tuple<>>::value));
  EXPECT_FALSE((contains<int, std::tuple<double, float, char>>::value));
  EXPECT_TRUE((contains<int, std::tuple<int>>::value));
  EXPECT_TRUE((contains<int, std::tuple<int, double>>::value));
  EXPECT_TRUE((contains<int, std::tuple<int, double, int>>::value));
  EXPECT_TRUE((contains<int, std::tuple<double, float, int>>::value));
}

}  // detail
}  // v1
}  // testing
