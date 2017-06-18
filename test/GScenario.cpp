//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GScenario.h"
#include "GUnit/GTest.h"
#include "GUnit/GMock.h"

#include "Common/Calculator.h"

class CalcSteps {
 public:
  $Given("^I have entered (\\d+) into the calculator$") = &CalcSteps::update;
  void update(double n) { calc.push(n); }

  $When("^I press add") = &CalcSteps::add;
  void add() { result = calc.add(); }

  $Given("^I press divide") = &CalcSteps::divide;
  void divide() { result = calc.divide(); }

  $Then("^the result should be (.*) on the screen$") = &CalcSteps::show;
  void show(double expected) { EXPECT_EQ(expected, result); }

 private:
  Calculator calc{};
  double result{};
};

$GScenario(CalcStepsLambda, "../test/Features/Calc/addition.feature")
class CalcStepsLambda {
 public:
  // clang-format off
  GIVEN("^I have entered (\\d+) into the calculator$")
    = [&](double n) { calc.push(n); };

  WHEN("^I press add")
    = [&] { result = calc.add(); };

  WHEN("^I press divide")
    = [&] { result = calc.divide(); };

  THEN("^the result should be (.*) on the screen$")
    = [&](double expected) { EXPECT_EQ(expected, result); };
  // clang-format on

 private:
  Calculator calc{};
  double result{};
};

GSCENARIO(CalcStepsMix, "../test/Features/Calc")  // run all features in the folder
class CalcStepsMix {
 public:
  // clang-format off
  $Given("^I have entered (\\d+) into the calculator$")
    = [&](double n) { calc.push(n); };

  $Given("^I press add") = &CalcSteps::add;
  void add() { result = calc.add(); }

  $When("^I press divide") = &CalcSteps::divide;
  void divide() { result = calc.divide(); }

  $Then("^the result should be (.*) on the screen$")
    = [&](double expected) { EXPECT_EQ(expected, result); };
  // clang-format on

 private:
  Calculator calc{};
  double result{};
};

class CalcStepsMock {
 public:
  // clang-format off
  $Given("^I have entered (\\d+) into the calculator$")
    = [&](double n) { calc.push(n); };

  $Given("^I press add") = &CalcStepsMock::add;
  void add() { calc.add(); }

  $When("^I press divide") = &CalcStepsMock::divide;
  void divide() { calc.divide(); }

  $Then("^the result should be (.*) on the screen$")
    = [&](double expected) { EXPECTED_CALL(display, (show)(expected)); };
  // clang-format on

 private:
  testing::GMock<IDisplay> display{DEFER_CALLS(IDisplay, show)};
  CalculatorUI calc{testing::object(display)};
};

GTEST("Calc features") {
  testing::RunScenario<CalcSteps>("../test/Features/Calc/addition.feature");
  testing::RunScenario<CalcStepsLambda>("../test/Features/Calc/addition.feature");
  testing::RunScenario<CalcStepsLambda>("../test/Features/Calc");
  testing::RunScenario<CalcStepsMix>("../test/Features/Calc/addition.feature;../test/Features/Calc/division.feature");
  testing::RunScenario<CalcStepsMix>("../test/Features/Calc/addition.feature", "../test/Features/Calc/division.feature");
  testing::RunScenario<CalcStepsMock>("../test/Features/Calc/addition.feature");
}
