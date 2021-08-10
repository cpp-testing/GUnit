//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GTest.h"
#include "GUnit/GAssert.h"

GTEST("Assert") {
  const auto b = true;
  const auto i = 42;

  EXPECT(true);
  EXPECT(b);
  EXPECT(42 == i);
  EXPECT(42 >= 0) << "message";

  ASSERT(42.0 == 42.);
}
