//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GSteps.h"
#include "GUnit/GTest.h"

// clang-format off
GSTEPS("Tags*") {
  using namespace testing;

  std::clog << Info() << std::endl;
  Given("I have a tag"_step) = [] { };

  std::clog << Info() << std::endl;
  When("I select a test with that tag"_step) = [] { };

  std::clog << Info() << std::endl;
  Then("I should see test being run"_step) = [] { };

  std::clog << Info() << std::endl;
  Then("I should see code being run"_step) = [] { };

  std::clog << Info() << std::endl;
}
// clang-format on
