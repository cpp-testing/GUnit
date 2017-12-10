//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GScenario.h"
#include <iostream>
#include <string>
#include <vector>
#include "GUnit/GTest.h"

STEPS("Example*") = [](auto steps) {
  std::vector<std::string> given_steps{};

  steps.Given("I have step {name}") = [&](const std::string& step) { given_steps.emplace_back(step); };

  steps.When("I run the scenario") = [] { std::cout << "Running...\n"; };

  steps.Then("The following steps should be shown on the screen", "steps") = [&](const testing::Table& table) {
    ASSERT_EQ(given_steps.size(), table.size());
    auto i = 0;
    for (auto row : table) {
      EXPECT_EQ(row["step"], given_steps[i++]);
    }
  };

  return steps;
};
