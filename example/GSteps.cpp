//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <iostream>
#include <string>
#include <vector>

#include "GUnit/GSteps.h"
#include "GUnit/GTest.h"

// clang-format off
GSTEPS("Example*") {
  auto step_count = 0;

  std::clog << "Set Up\n" << Info() << std::endl;

  EXPECT_EQ(0, step_count++);

  Given("I create steps") =
   [&] {
     std::vector<std::string> given_steps{};
     EXPECT_EQ(1, step_count++);

     Given("I add step {name}") =
       [&](const std::string& name) {
         given_steps.push_back(name);
         EXPECT_TRUE(2 == step_count || 3 == step_count);
         ++step_count;
       };

     When("I run the scenario") =
       [&] {
         std::cout << "Running...\n";
         EXPECT_EQ(4, step_count++);
       };

     Then("The following steps should be shown on the screen", "{steps}") =
      [&](const testing::Table& table) {
        EXPECT_EQ(5, step_count++);
        ASSERT_EQ(given_steps.size(), table.size());
        auto i = 0;
        for (auto row : table) {
          EXPECT_EQ(row["step"], given_steps[i++]);
        }
      };
  };

  EXPECT_EQ(6, step_count);

  std::clog << "Tear Down\n" << Info() << std::endl;
}
// clang-format on
