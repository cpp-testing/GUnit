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
STEPS("*") = [](auto& scenario) {
  using namespace testing;
  Steps steps{scenario};
  Calculator calc{};
  double result{};

  steps.Given("I have entered {n} into the calculator"_s) =
    [&](double n) {
      calc.push(n);
    };

  steps.When("I press add"_s) =
    [&] {
      result = calc.add();
    };

  steps.Given("I press divide"_s) =
    [&] {
      result = calc.divide();
    };

  steps.Then("the result should be {expected} on the screen"_s) =
    [&](double expected) {
      EXPECT_EQ(expected, result);
    };

  return steps;
};

STEPS("*") = [](auto& scenario) {
  testing::GMock<IDisplay> display{DEFER_CALLS(IDisplay, show)};
  CalculatorUI calc{testing::object(display)};
  testing::Steps steps{scenario};

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

  return steps;
};
// clang-format on
