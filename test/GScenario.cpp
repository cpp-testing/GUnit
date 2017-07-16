//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GScenario.h"
#include "GUnit/GTest.h"

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

