//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GScenario.h"
#include "GUnit/GTest.h"

// clang-format off
STEPS("Tags*") = [](testing::Steps steps) {
  using namespace testing;
  steps.Given("I have a tag"_step) = [] {};
  steps.When("I select a test with that tag"_step) = [] {};
  steps.Then("I should see test being run"_step) = [] {};
  return steps;
};
// clang-format on
