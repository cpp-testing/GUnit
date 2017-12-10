//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GSteps.h"
#include "GUnit/GTest.h"

namespace testing {
inline namespace v1 {

GSTEPS("*") {
  using namespace testing;
  try {
    Given("{}") = [](int) {};
    $Given("{} {}") = [](int, double) {};
    Given("{n}") = [](int) {};
    Given("{n}") = [](std::string) {};
    Given("{n}") = [](const std::string&) {};
    Given("{a value}") = [](int) {};
    $When("", "table") = [](const Table&) {};
    When("", "table") = [](Table) {};
    Then("{}", "table") = [](int, const Table&) {};
    Then("{}", "table") = [](const std::string&, const Table&) {};
    $Then("{}", "table") = [](int, Table) {};
  } catch (...) {
  }
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
