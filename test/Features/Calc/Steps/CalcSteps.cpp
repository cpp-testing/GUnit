//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GMock.h"
#include "GUnit/GSteps.h"
#include "GUnit/GTest.h"

#include "Features/Calc/Impl/Calculator.h"

// clang-format off
GSTEPS("Calc Addition.Add two numbers") {
  using namespace testing;
  Calculator calc{};
  double result{};

  Given("I created a calculator with initial value equals {n}"_step) =
    [&](double) { /*ignore*/ };

  Given("I have entered {n} into the calculator"_step) =
    [&](double n) {
      calc.push(n);
    };

  When("I press add"_step) =
    [&] {
      result = calc.add();
    };

  Then("the result should be {expected} on the screen"_step) =
    [&](double expected) {
      EXPECT_EQ(expected, result);
    };
}

GSTEPS("Calc *") {
  using namespace testing;
  double result{};

  Given("I created a calculator with initial value equals {n}"_step) =
    [&](double n) {
      Calculator calc{n};

      Given("I have entered {n} into the calculator"_step) =
        [&](double n) {
          calc.push(n);
        };

      When("I press add"_step) =
        [&] {
          result = calc.add();
        };

      When("I press divide"_step) =
        [&] {
          result = calc.divide();
        };

      Then("the result should be {expected} on the screen"_step) =
        [&](double expected) {
          EXPECT_EQ(expected, result);
        };
    };
}

GSTEPS("Calc*") {
  using namespace testing;
  Calculator calc{};
  auto result = 0.;
  auto step = 0;

  std::clog << "SetUp" << std::endl;
  EXPECT_EQ(0, step++);

  Given("I created a calculator with initial value equals {n}"_step) =
    [&](double) {
      EXPECT_EQ(1, step++);
    };

  Given("I have entered {n} into the calculator"_step) =
    [&](double n) {
      calc.push(n);
      EXPECT_TRUE(2 == step || 3 == step);
      ++step;
    };

  When("I press add"_step) =
    [&] {
      result = calc.add();
      EXPECT_EQ(4, step++);
    };

  When("I press divide"_step) =
    [&] {
      result = calc.divide();
      EXPECT_EQ(4, step++);
    };

  Then("the result should be {expected} on the screen"_step) =
    [&](double expected) {
      EXPECT_EQ(expected, result);
      EXPECT_EQ(5, step++);
    };

  EXPECT_EQ(6, step);
  std::clog << "TearDown" << std::endl;;
}

GSTEPS("Calc*") {
  using namespace ::testing;
  testing::GMock<IDisplay> display{DEFER_CALLS(IDisplay, show)};
  CalculatorUI calc{testing::object(display)};

  Given("I created a calculator with initial value equals {n}"_step) =
    [&](double) { /*ignore*/ };

  Given("I have entered {n} into the calculator") =
    [&](double n) {
      calc.push(n);
    };

  When("I press add") =
    [&]{ calc.add(); };

  Given("I press divide") =
    [&]{ calc.divide(); };

  Then("the result should be {expected} on the screen") =
    [&] (double expected) {
      EXPECT_CALL(display, (show)(expected));
    };
}

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

GSTEPS("Calc*") {
  using namespace testing;
  Calculator calc{};
  double result{};

  Given("I created a calculator with initial value equals {n}") = [&](double) { /*ignore*/ };
  Given("I have entered {n} into the calculator")        = CalcPush(calc);
   When("I press add")                                   = CalcAdd(calc, result);
   Then("the result should be {expected} on the screen") = CalcResult(result);
   When("I press divide")                                = CalcDivide(calc, result);
}

auto when_steps = [](auto& steps, auto& calc, auto& result) {
  using namespace testing;
  steps.When("I press add")    = CalcAdd(calc, result);
  steps.When("I press divide") = CalcDivide(calc, result);
};

GSTEPS("Calc*") {
  using namespace testing;
  Given("I created a calculator with initial value equals {n}") = [&](double n) {
    double result{};
    Calculator calc{n};

    when_steps(*this, calc, result);
    Given("I have entered {n} into the calculator")        = CalcPush(calc);
    Then ("the result should be {expected} on the screen") = CalcResult(result);
  };
}
// clang-format on
