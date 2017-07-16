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
STEPS("Table") = [](testing::Steps steps) {
  using namespace testing;
  Table expected_table{};
  std::string expected_desc{};

  steps.$Given("I have the following table"_step, "ids") =
    [&](const Table& table) {
      expected_table = table;
    };

  steps.$When("I choose {id}"_step) =
    [&](int id) {
      for (auto row : expected_table) {
          if (row["id"] == std::to_string(id)) {
              expected_desc = row["desc"];
              break;
          }
      }
    };

  steps.$Then("I should get '{desc}'"_step) =
    [&](const std::string& desc) {
      EXPECT_EQ(expected_desc, desc);
    };

  return steps;
};

struct Context {
  testing::Table expected_table{};
  std::string expected_desc{};
};

STEPS("Table") = [](testing::Steps steps, Context ctx) {
  steps.$Given("I have the following table", "ids") =
    [&](testing::Table ids) {
      ctx.expected_table = ids;
    };

  steps.$When("I choose {id}") =
    [&](int id) {
      for (auto row : ctx.expected_table) {
          if (row["id"] == std::to_string(id)) {
              ctx.expected_desc = row["desc"];
              break;
          }
      }
    };

  steps.$Then("I should get '{desc}'") =
    [&](const std::string& desc) {
      EXPECT_EQ(ctx.expected_desc, desc);
    };

  return steps;
};
// clang-format on
