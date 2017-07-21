//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GScenario.h"
#include "GUnit/GTest.h"

namespace testing {
inline namespace v1 {
GTEST("Steps") {
  using namespace testing;
  testing::Steps steps{{}, {}};
  steps.Given("{}") = [](int) {};
  steps.$Given("{} {}") = [](int, double) {};
  steps.Given("{n}") = [](int) {};
  steps.Given("{n}") = [](std::string) {};
  steps.Given("{n}") = [](const std::string&) {};
  steps.Given("{a value}") = [](int) {};
  steps.$When("", "table") = [](const Table&) {};
  steps.When("", "table") = [](Table) {};
  steps.Then("{}", "table") = [](int, const Table&) {};
  steps.Then("{}", "table") = [](const std::string&, const Table&) {};
  steps.$Then("{}", "table") = [](int, Table) {};
}

GTEST("Table") {
  SHOULD("make empty table from empty json") {
    nlohmann::json json{};
    json["arguments"] = {};
    EXPECT_TRUE(detail::make_table(json).empty());
  }

  SHOULD("make one row table from json") {
    // clang-format off
    const auto json = R"({
       "arguments":[
          {
             "rows":[
                {
                   "cells":[
                      {
                         "location":{
                            "column":9,
                            "line":5
                         },
                         "value":"foo"
                      },
                      {
                         "location":{
                            "column":15,
                            "line":5
                         },
                         "value":"bar"
                      }
                   ]
                },
                {
                   "cells":[
                      {
                         "location":{
                            "column":9,
                            "line":6
                         },
                         "value":"boz"
                      },
                      {
                         "location":{
                            "column":15,
                            "line":6
                         },
                         "value":"boo"
                      }
                   ]
                }
             ]
          }
       ],
       "locations":[
          {
             "column":11,
             "line":4
          }
       ],
       "text":"a simple data table"
    })"_json;
    // clang-format on

    auto table = detail::make_table(json);
    ASSERT_EQ(1u, table.size());
    EXPECT_EQ("boz", table[0]["foo"]);
    EXPECT_EQ("boo", table[0]["bar"]);
  }

  SHOULD("make many rows table from json") {
    // clang-format off
    const auto json = R"({
       "arguments":[
          {
             "rows":[
                {
                   "cells":[
                      {
                         "location":{
                            "column":9,
                            "line":5
                         },
                         "value":"foo"
                      },
                      {
                         "location":{
                            "column":15,
                            "line":5
                         },
                         "value":"bar"
                      }
                   ]
                },
                {
                   "cells":[
                      {
                         "location":{
                            "column":9,
                            "line":6
                         },
                         "value":"boz"
                      },
                      {
                         "location":{
                            "column":15,
                            "line":6
                         },
                         "value":"boo"
                      }
                   ]
                },
                {
                   "cells":[
                      {
                         "location":{
                            "column":9,
                            "line":6
                         },
                         "value":"boz2"
                      },
                      {
                         "location":{
                            "column":15,
                            "line":6
                         },
                         "value":"boo2"
                      }
                   ]
                }
             ]
          }
       ],
       "locations":[
          {
             "column":11,
             "line":4
          }
       ],
       "text":"a simple data table"
    })"_json;
    // clang-format on

    auto table = detail::make_table(json);
    ASSERT_EQ(2u, table.size());
    EXPECT_EQ("boz", table[0]["foo"]);
    EXPECT_EQ("boo", table[0]["bar"]);
    EXPECT_EQ("boz2", table[1]["foo"]);
    EXPECT_EQ("boo2", table[1]["bar"]);
  }
}

}  // v1
}  // testing
