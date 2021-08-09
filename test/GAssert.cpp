//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <gtest/gtest.h>
#include "GUnit/GAssert.h"

TEST(GAssert, ShouldSupportExpect) {
  auto i = 42;
  const auto b = true;

  EXPECT(true);
  EXPECT(!false) << "message";
  EXPECT(b);

  EXPECT(i == 42);
  EXPECT(42 == i);
  EXPECT(42 == i);

  EXPECT(i != 0);
  EXPECT(0 != i);

  EXPECT(i > 0) << "message";
  EXPECT(i >= 0) << "message";
  EXPECT(i <= 42) << "message";
  EXPECT(i < 100) << "message";

  EXPECT(42.0 == 42.);
  EXPECT(2*21 == i);
}

TEST(GAssert, ShouldSupportASSERT) {
  const auto i = 42;

  ASSERT(true);

  ASSERT(i == 42);
  ASSERT(42 == i);

  ASSERT(i != 0);
  ASSERT(0 != i);

  ASSERT(i > 0) << "message";
  ASSERT(i >= 0) << "message";
  ASSERT(i <= 42) << "message";
  ASSERT(i < 100) << "message";

  ASSERT(42.0 == 42.);
  ASSERT(2*21 == i);

  ASSERT("42" == std::string{"42"});
  ASSERT(std::string{"42"} == "42");
  ASSERT("42" != std::string{});
  ASSERT(std::string{} != "42");
}
