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

#include "Features/Calc/Impl/Calculator.h"

// clang-format off
STEPS("Calc *") = [](auto steps) {
  using namespace testing;
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

  return steps;
};

STEPS("Calc *") = [](auto steps, Calculator calc, double result) {
  using namespace testing;
  auto b = 0;
  auto a = 0;
  auto first = 0;

  steps.Before() = [&]{ ++b; };
  steps.After() = [&]{ ++a; };

  steps.Given("I have entered {n} into the calculator"_step) =
    [&](double n) {
      calc.push(n);
      EXPECT_EQ(1 + first, b);
      EXPECT_EQ(0 + first, a);
      ++first;
    };

  steps.When("I press add"_step) =
    [&] {
      result = calc.add();
      EXPECT_EQ(3, b);
      EXPECT_EQ(2, a);
    };

  steps.When("I press divide"_step) =
    [&] {
      result = calc.divide();
      EXPECT_EQ(3, b);
      EXPECT_EQ(2, a);
    };

  steps.Then("the result should be {expected} on the screen"_step) =
    [&](double expected) {
      EXPECT_EQ(expected, result);
      EXPECT_EQ(4, b);
      EXPECT_EQ(3, a);
    };

  return steps;
};

STEPS("Calc*") = [](auto steps) {
  testing::GMock<IDisplay> display{DEFER_CALLS(IDisplay, show)};
  CalculatorUI calc{testing::object(display)};

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
      EXPECT_CALL(display, (show)(expected));
    };

  return steps;
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

STEPS("Calc *") = [](auto steps) {
  Calculator calc{};
  double result{};

  steps.Given("I have entered {n} into the calculator")        = CalcPush(calc);
  steps.When ("I press add")                                   = CalcAdd(calc, result);
  steps.When ("I press divide")                                = CalcDivide(calc, result);
  steps.Then ("the result should be {expected} on the screen") = CalcResult(result);

  return steps;
};

STEPS("Calc*") = [](auto steps, Calculator calc, double result) {
  steps.Given("I have entered {n} into the calculator")        = CalcPush(calc);
  steps.When ("I press add")                                   = CalcAdd(calc, result);
  steps.When ("I press divide")                                = CalcDivide(calc, result);
  steps.Then ("the result should be {expected} on the screen") = CalcResult(result);
  return steps;
};

auto common_steps = [](auto& steps, auto& calc, auto& result) {
  steps.When ("I press add")                                   = CalcAdd(calc, result);
  steps.When ("I press divide")                                = CalcDivide(calc, result);
};

STEPS("Calc*") = [](auto steps, Calculator calc, double result) {
  steps.Given("I have entered {n} into the calculator")        = CalcPush(calc);
  steps.Then ("the result should be {expected} on the screen") = CalcResult(result);
  common_steps(steps, calc, result);
  return steps;
};
