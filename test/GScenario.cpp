//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GScenario.h"
#include "GUnit/GMock.h"
#include "GUnit/GTest.h"

#include "Common/Calculator.h"

// clang-format off
STEPS("Calc *") = [](auto& scenario) {
  using namespace testing;
  Steps steps{};
  Calculator calc{};
  double result{};

  steps.Given("I have entered {n} into the calculator"_step) =
    [&](double n) {
      calc.push(n);
    };

  steps.When("I press add"_step) =
    [&] {
      result = calc.add();
    };

  steps.When("I press divide"_step) =
    [&] {
      result = calc.divide();
    };

  steps.Then("the result should be {expected} on the screen"_step) =
    [&](double expected) {
      EXPECT_EQ(expected, result);
    };

  return steps(scenario);
};

STEPS("Calc*") = [](auto& scenario) {
  testing::GMock<IDisplay> display{DEFER_CALLS(IDisplay, show)};
  CalculatorUI calc{testing::object(display)};
  testing::Steps steps{};

  steps.Given("I have entered {n} into the calculator") =
    [&](double n) {
      calc.push(n);
    };

  steps.When("I press add") =
    [&]{ calc.add(); };

  steps.Given("I press divide") =
    [&]{ calc.divide(); };

  steps.Then("the result should be {expected} on the screen") =
    [&] (double expected) {
      EXPECTED_CALL(display, (show)(expected));
    };

  return steps(scenario);
};

const auto CalcPush = [](auto& calc) {
  return [&](double n) {
    calc.push(n);
  };
};

const auto CalcAdd = [](auto& calc, auto& result) {
  return [&] {
    result = calc.add();
  };
};

const auto CalcDivide = [](auto& calc, auto& result) {
  return [&] {
    result = calc.divide();
  };
};

const auto CalcResult = [](auto& result) {
  return [&](double expected) {
    EXPECT_EQ(expected, result);
  };
};

STEPS("Calc *") = [](auto& scenario) {
  testing::Steps steps{};
  Calculator calc{};
  double result{};

  steps.Given("I have entered {n} into the calculator")        = CalcPush(calc);
  steps.When ("I press add")                                   = CalcAdd(calc, result);
  steps.When ("I press divide")                                = CalcDivide(calc, result);
  steps.Then ("the result should be {expected} on the screen") = CalcResult(result);

  return steps(scenario);
};

STEPS("Table") = [](auto& scenario) {
  using namespace testing;
  Steps steps{};
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
    [&](std::string desc) {
      EXPECT_EQ(expected_desc, desc);
    };

  return steps(scenario);
};
// clang-format on
