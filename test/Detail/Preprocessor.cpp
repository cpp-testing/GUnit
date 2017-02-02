//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/Detail/Preprocessor.h"
#include <gtest/gtest.h>

TEST(Preprocessor, ShouldReturnSize) {
  EXPECT_EQ(1, (__GUNIT_SIZE(a)));
  EXPECT_EQ(2, (__GUNIT_SIZE(a, b)));
}

TEST(Preprocessor, ShouldReturnTrueIfIsAParen) {
  EXPECT_FALSE(__GUNIT_IBP());
  EXPECT_FALSE(__GUNIT_IBP(a));
  EXPECT_TRUE(__GUNIT_IBP((a)));
  EXPECT_TRUE(__GUNIT_IBP((a, b)));
}

TEST(Preprocessor, ShouldReturnTrueIfIsEmpty) {
  EXPECT_TRUE(__GUNIT_IS_EMPTY());
  EXPECT_FALSE(__GUNIT_IS_EMPTY(a));
  EXPECT_FALSE(__GUNIT_IS_EMPTY(a, b));
  EXPECT_FALSE(__GUNIT_IS_EMPTY(a, b, c));
}

TEST(Preprocessor, ShouldReturnConditionalValue) {
  EXPECT_TRUE(__GUNIT_IF(1)(true, false));
  EXPECT_FALSE(__GUNIT_IF(0)(true, false));
}

TEST(Preprocessor, ShouldReturnMergedValue) {
  __GUNIT_CAT(int a, b) = 0;
  EXPECT_EQ(0, ab);
}
