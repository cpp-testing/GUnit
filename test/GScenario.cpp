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
STEPS("*") calcSteps = [] {
  Calculator calc{};
  double result{};

  $Given("I have entered {n} into the calculator") =
    [&](double n) {
      calc.push(n);
    };

  $When("I press add") =
    [&]{
      result = calc.add();
    };

  $Given("I press divide") =
    [&]{
      result = calc.divide();
    };

  $Then("the result should be {expected} on the screen") =
    [&] (double expected) {
      EXPECT_EQ(expected, result);
    };
};

STEPS("*") calcStepsMock = [] {
  testing::GMock<IDisplay> display{DEFER_CALLS(IDisplay, show)};
  CalculatorUI calc{testing::object(display)};

  $Given("I have entered {n} into the calculator") =
    [&](double n) {
      calc.push(n);
    };

  $When("I press add") =
    [&]{ calc.add(); };

  $Given("I press divide") =
    [&]{ calc.divide(); };

  $Then("the result should be {expected} on the screen") =
    [&] (double expected) {
      EXPECTED_CALL(display, (show)(expected));
    };
};

// clang-format on
