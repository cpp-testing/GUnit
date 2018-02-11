//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <vector>

#include "GUnit/GSteps.h"
#include "GUnit/GTest.h"

const auto access_data = [](testing::Data id_value) {
  {
    const int id = id_value["id"];
    EXPECT_EQ(42, id);
  }

  {
    const std::string value = id_value["value"];
    EXPECT_EQ("number", value);
  }

  // access not-defined data
  {
    EXPECT_FALSE(id_value["not-defined"].available());
    const std::string value = id_value["not-defined"];
    EXPECT_EQ("", value);
    EXPECT_TRUE(id_value["id"].available());
    EXPECT_FALSE(id_value["id"].empty());
  }
};

// clang-format off
GSTEPS("Table.Table*") {
  using namespace testing;
  Table expected_table{};
  std::string expected_desc{};

  $Given("I have the following table"_step, "ids") =
    [&](const Table table) {
      expected_table = table;
    };

  $Given("I access table with 1 row, 1 col"_step, "ids") = access_data;

  $Given("I access table with {n}", "ids") = [](int n, const Data& data) {
    EXPECT_EQ(n, int(data["value"]));
  };

  $When("I choose {id}"_step) =
    [&](int id) {
      for (auto row : expected_table) {
        if (row["id"] == std::to_string(id)) {
          expected_desc = row["desc"];
          break;
        }
      }
    };

  $Then("I should get '{desc}'"_step) =
    [&](const std::string& desc) {
      EXPECT_EQ(expected_desc, desc);
    };
}

struct Context {
  testing::Table expected_table{};
  std::string expected_desc{};
};

GSTEPS("Table.Table*") {
  Context ctx{};

  $Given("I have the following {table}", "ids") =
    [&](const std::string& txt, testing::Table ids) {
      EXPECT_EQ("table", txt);
      ctx.expected_table = ids;
    };

  $Given("I access table with {row} row, {col} col", "ids") =
    [](const int row, const int col, testing::Data id_value) {
      EXPECT_EQ(1, row);
      EXPECT_EQ(1, col);
      access_data(id_value);
    };

  $Given("I access table with {n}", "ids") = [](int n, testing::Data data) {
    EXPECT_EQ(n, int(data["value"]));
  };

  $When("I choose {id}") =
    [&](int id) {
      for (auto row : ctx.expected_table) {
        if (row["id"] == std::to_string(id)) {
          ctx.expected_desc = row["desc"];
          break;
       }
      }
    };

  $Then("I should get '{desc}'") =
    [&](const std::string& desc) {
      EXPECT_EQ(ctx.expected_desc, desc);
    };
}

GSTEPS("Table.Conversions*") {
  using namespace testing;

  Given("I have a table", "data") = [&](Table values) {
    std::vector<bool> bools{};
    for (auto& value : values) {
      bools.push_back(value["value"]);
    }

    When("I do conversion to boolean") = [&] {
      Then("I should get", "data") = [&](Table values) {
        auto i = 0;
        for (auto& value : values) {
          EXPECT_EQ(bool(value["value"]), bools[i++]);
        }
      };
    };
  };
}
// clang-format on
