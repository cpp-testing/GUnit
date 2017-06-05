//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GTest.h"
#include "GUnit/GScenario.h"

#include "Common/Calculator.h"

class CalcSteps {
public:
  GIVEN("^I have entered (\\d+) into the calculator$") = &CalcSteps::update;
  void update(double n) {
    calc.push(n);
  }

  WHEN("^I press add") = &CalcSteps::add;
  void add() {
    result = calc.add();
  }

  WHEN("^I press divide") = &CalcSteps::divide;
  void divide() {
    result = calc.divide();
  }

  THEN("^the result should be (.*) on the screen$") = &CalcSteps::show;
  void show(double expected) {
    EXPECT_EQ(expected, result);
  }

private:
  Calculator calc{};
  double result{};
};

class CalcStepsLambda {
public:
  GIVEN("^I have entered (\\d+) into the calculator$")
    = [&](double n) { calc.push(n); };

  WHEN("^I press add")
    = [&] { result = calc.add(); };

  WHEN("^I press divide")
    = [&] { result = calc.divide(); };

  THEN("^the result should be (.*) on the screen$")
    = [&](double expected) { EXPECT_EQ(expected, result); };

private:
  Calculator calc{};
  double result{};
};

GTEST("Calc features") {
  testing::Scenario<CalcSteps>{"../test/Features/addition.feature"};
  testing::Scenario<CalcStepsLambda>{"../test/Features/addition.feature"};
}
