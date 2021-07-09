//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GAssert.h"

int main() {
  EXPECT(true);

  const auto i = 42;
  EXPECT(42 == i);

  ASSERT(42.0 == 42.);
}
