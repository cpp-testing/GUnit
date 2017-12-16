//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <iostream>
#include <sstream>

#include "GUnit/GSteps.h"
#include "GUnit/GTest.h"

// clang-format off
GSTEPS("Data*") {
  using namespace testing;

  std::string text{};
  std::stringstream str{};

  Given("I have a multiline text", "text") = [&](const Data& data) {
    text = data.text;
  };

  When("I print it") = [&] {
    str << text;
    std::clog << str.str() << std::endl;
  };

  Then("I should see", "text") = [&](const Data& data) {
    EXPECT_EQ(data.text, str.str());
  };
}
// clang-format on
