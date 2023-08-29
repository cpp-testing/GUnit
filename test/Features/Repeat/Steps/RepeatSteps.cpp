//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GSteps.h"
#include "GUnit/GAssert.h"

GSTEPS("Repeat*") {
  int id{};
  Given("I have a text", "{steps}") = [&](const testing::Table& table) {
    EXPECT(id++ == int(table["id"]));
  };
  Given("I print it") = [] { };
  Given("I should see", "{steps}") = [&](const testing::Table& table) {
    EXPECT(id++ == int(table["id"]));
  };
}
