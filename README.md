<a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a>
<a href="https://github.com/cpp-testing/GUnit/releases" target="_blank">![Version](https://badge.fury.io/gh/cpp-testing%2FGUnit.svg)</a>
<a href="https://travis-ci.org/cpp-testing/GUnit" target="_blank">![Build Status](https://img.shields.io/travis/cpp-testing/GUnit/master.svg?label=linux/osx)</a>
<a href="http://github.com/cpp-testing/GUnit/issues" target="_blank">![Github Issues](https://img.shields.io/github/issues/cpp-testing/GUnit.svg)</a>

---

## Testing

> "If you liked it then you should have put a test on it", Beyonce rule

## GUnit
> [Google.Test/Google.Mock](https://github.com/google/googletest)/[Cucumber](https://github.com/cucumber/cucumber/wiki/Gherkin) on steroids

* Improve your productivity with GUnit, a library which extends/simplifies [Google.Test/Google.Mock](https://github.com/google/googletest) and adds support for [Gherkin](https://github.com/cucumber/cucumber/wiki/Gherkin) (Behaviour Driven Development) to it.

## Motivation

> ### No more base classes and labels as identifiers - [GUnit.GTest](docs/GTest.md) / [GUnit.GTest-Lite](docs/GTest-Lite.md)
  ```cpp
                 Google.Test                        |                     GUnit.GTest
  --------------------------------------------------+------------------------------------------------------
  #include <gtest/gtest.h>                          | #include <Gunit.h>
                                                    |
  struct CalcTest : testing::Test {                 | GTEST("Calc Test") {
   void SetUp() override {                          |   Calc calc{...};
     calc = std::make_unique<Calc>(...);            |
   }                                                |   // SetUp
                                                    |
   void TearDown() override { }                     |   SHOULD("return sum of 2 numbers") {
                                                    |     EXPECT_EQ(5, calc->add(4, 1));
   std::unique_ptr<Calc> calc;                      |   }
  };                                                |
                                                    |   SHOULD("throw if division by 0") {
  TEST_F(ExampleTest, ShouldReturnSumOf2Numbers) {  |     EXPECT_ANY_THROW(calc->div(42, 0));
    EXPECT_EQ(5, calc->add(4, 1));                  |   }
  }                                                 |
                                                    |   // TearDown
  TEST_F(ExampleTest, ShouldThrowIfDivisionBy0) {   | }
    EXPECT_ANY_THROW(calc->div(42, 0));             |
  }                                                 |
  ```

  > Output
  ```sh
  [----------] 2 tests from CalcTest                | [----------] 1 tests from Calc Test
  [ RUN      ] CalcTest.ShouldReturnSumOf2Numbers   | [ RUN      ] Calc Test
  [       OK ] CalcTest.ShouldReturnSumOf2Numbers   | [ SHOULD   ] return sum of 2 numbers
  [ RUN      ] CalcTest.ShouldThrowIfDivisionBy0    | [ SHOULD   ] throw if division by 0
  [       OK ] CalcTest.ShouldThrowIfDivisionBy0    | [       OK ] Calc Test (0 ms)
  [----------] 2 tests from CalcTest (1 ms total)   | [----------] 1 tests from Example (0 ms total)
  ```

> ### No more hand written mocks - [GUnit.GMock](docs/GMock.md)
  ```cpp
  struct interface {
    virtual ~interface() = default;
    virtual int get() const = 0;
    virtual void foo(int) = 0;
    virtual void bar(int, const std::string&) = 0;
  };
  ```
  ```cpp
                 Google.Test                        |                     GUnit.GMock
  --------------------------------------------------+------------------------------------------------------
  #include <gmock/gmock.h>                          | #include <Gunit.h>
                                                    |
  struct mock_interface : interface {               |
    MOCK_CONST_METHOD0(get, int(int));              |
    MOCK_METHOD1(foo, void(int));                   |
    MOCK_METHOD2(bar, void(int, const string&));    |
  };                                                |
                                                    |
  int main() {                                      | int main() {
    StrictMock<mock_interface> mock{};              |   StrictGMock<interface> mock{};
    EXPECT_CALL(mock, foo(42));                     |   EXPECT_CALL(mock, (foo)(42));
                                                    |
    interface& i = mock;                            |   interface& i = mock.object();
    i.foo(42);                                      |   i.foo(42);
  }                                                 | }
  ```

> ### Simplified creation and injection of SUT (System Under Test) and mocks - [GUnit.GMake](docs/GMake.md)
  ```cpp
  class coffee_maker {
   public:
     coffee_maker(iheater&, ipump&, igrinder&);
     ...
  };
  ```
  ```cpp
                 Google.Test                        |                     GUnit.GMake
  --------------------------------------------------+------------------------------------------------------
   #include <gtest/gtest.h>                         | #include <Gunit.h>
   #include <gmock/gmock.h>                         |
                                                    |
   TEST(CalcTest, ShouldMakeCoffee) {               | GTEST("Calc Test") {
     StrictMock<mock_heater> heater{};              |   std::tie(sut, mocks) = // auto [sut, mocks] in C++17
     StrictMock<mock_pump> pump{};                  |     make<coffee_maker, StrictGMock>();
     StrictMock<mock_grinder> grinder{};            |
     coffee_maker sut{heater, pump, grinder};       |   EXPECT_CALL(mocks.mock<iheater>(), (on)());
                                                    |   EXPECT_CALL(mocks.mock<ipump>(), (pump)());
     EXPECT_CALL(heater, on());                     |   EXPECT_CALL(mocks.mock<igrinder>(), (grind)());
     EXPECT_CALL(pump, pump());                     |   EXPECT_CALL(mocks.mock<iheater>(), (off)());
     EXPECT_CALL(grinder, grind());                 |
     EXPECT_CALL(heater, off());                    |   sut->brew();
                                                    | }
     sut->brew();                                   |
   }
  ```

> ### Support for - [Gherkin](https://github.com/cucumber/cucumber/wiki/Gherkin) style - BDD (Behaviour Driven Development) scenarios - [GUnit.GSteps](docs/GSteps.md)

  > Feature specification

    Test/Features/Calc/addition.feature ([Gherkin](https://github.com/cucumber/cucumber/wiki/Gherkin))

  ```gherkin
  Feature: Calc Addition
    In order to avoid silly mistakes
    As a math idiot
    I want to be told the sum of two numbers

    Scenario: Add two numbers
      Given I created a calculator with value 0
        And I have entered 20 into the calculator
        And I have entered 30 into the calculator
       When I press add
       Then The result should be 50
  ```

  > Steps Implementation

    Test/Features/Calc/Steps/CalcSteps.cpp

  ```cpp
  #include <Gunit.h>

  GSTEPS("Calc*") { // "Calc Addition.Add two numbers"
    auto result = 0;

    Given("I created a calculator with value {n}") =
      [&](int n) {
        Calculator calc{n};

        Given("I have entered {n} into the calculator") =
         [&](int n) { calc.push(n); };

        When("I press add") =
         [&] { result = calc.add(); };

        Then("The result should be {expected}") =
         [&](int expected) {
           EXPECT_EQ(expected, result);
         };
      };
  }
  ```

  > Usage
  ```sh
  SCENARIO="Test/Features/Calc/addition.feature" ./test --gtest_filter="Calc Addition.Add two numbers"
  ```

  > Output
  ```sh
  [==========] Running 1 test from 1 test case.
  [----------] Global test environment set-up.
  [----------] 1 tests from Calc Addition
  [ RUN      ] Calc Addition.Add two numbers
  [    Given ] I have created a calculator with value 0         # CalcSteps.cpp:10
  [    Given ] I have entered 20 into the calculator            # CalcSteps.cpp:12
  [    Given ] I have entered 30 into the calculator            # CalcSteps.cpp:14
  [     When ] I press add                                      # CalcSteps.cpp:16
  [     Then ] the result should be 50 on the screen            # CalcSteps.cpp:19
  [----------] Global test environment tear-down
  [==========] 1 test from 1 test case ran. (7 ms total)
  [  PASSED  ] 1 tests.
  ```

## Overview
* Extensions (independent)
  * `GUnit.GTest` - Google.Test with strings and more friendly macros
    * Test cases with string as names
    * No more SetUp/TearDown (SHOULD clauses)
    * One (GTEST) macro for all types of tests
    * 100% Compatible with tests using GTest
  * `GUnit.GTest-Lite` - lightweight, limited, no-macro way of defining simple tests
  * `GUnit.GMock` - Google.Mock without hand written mocks
    * No more hand written mocks!
    * Support for more than 10 parameters
    * Quicker compilation times
    * Support for unique_ptr without any tricks
    * Support for overloaded operators
    * Support for mocking classes with constructors
    * 100% Compatible with Google Mocks
  * `GUnit.GMake` - Makes creation of System Under Test (SUT) and Mocks easier
    * No need to instantiate SUT (System Under Test) and mocks
      * Automatic mocks injection
  * `GUnit.GSteps` - Behaviour Driven Development
    * Support for - [Gherkin](https://github.com/cucumber/cucumber/wiki/Gherkin) style - BDD tests
* Based on top of [Google.Test/Google.Mock](https://github.com/google/googletest)
    * (+) Widely used (The most popular testing framework according to https://www.jetbrains.com/research/devecosystem-2017/cpp)
    * (+) Stable
    * (+) Powerful
    * (+) Comes with GoogleMock
    * (+) Well documented
    * (-) No support for - [Gherkin](https://github.com/cucumber/cucumber/wiki/Gherkin) style - BDD tests
    * (-) Boilerplate macros
* Requirements
  * [C++14](https://ubershmekel.github.io/cppdrafts/c++14-cd.html)
  * `GTest/GTest-Lite/GMock/GMake`
    * [libs/googletest](https://github.com/google/googletest) - compatible with all versions
  * `GSteps`
    * [libs/json](https://github.com/nlohmann/json)
    * [libs/gherkin-cpp](https://github.com/c-libs/gherkin-cpp)
        * Building libgherkin-cpp
          ```
          $cd libs/gherkin-cpp && make lib
          $ls libs/gherkin-cpp
            libgherkin-cpp.a
            libgherkin-cpp.so
          ```
* Tested compilers
  * [Linux - GCC-5+](https://travis-ci.org/cpp-testing/GUnit)
  * [Linux - Clang-3.7+](https://travis-ci.org/cpp-testing/GUnit)
  * [Mac OSx - Xcode-7.3+](https://travis-ci.org/cpp-testing/GUnit)
* Quick start
  ```sh
  $mkdir build && cd build && cmake ..
  $make && ctest
  ```
* User Guide
  * [GUnit.GTest](docs/GTest.md)
  * [GUnit.GTest-Lite](docs/GTest-Lite.md)
  * [GUnit.GMock](docs/GMock.md)
  * [GUnit.GMake](docs/GMake.md)
  * [GUnit.GSteps](docs/GSteps.md)
* [FAQ](docs/FAQ.md)
  * [C++ Now 2017: Towards Painless Testing](https://www.youtube.com/watch?v=NVrZjT5lW5o)

### Acknowledgements
* Thanks to [Google Open Source](https://opensource.google.com/) for [Google.Test/Google.Mock](https://github.com/google/googletest) libraries
* Thanks to Eran Pe'er for [FakeIt](https://github.com/eranpeer/FakeIt) library
* Thanks to Peter Bindels for [HippoMocks](https://github.com/dascandy/hippomocks) library
* Thanks to Niels Lohmann for [json](https://github.com/nlohmann/json) library
* Thanks to Aslak Hellesøy for [gherkin-c](https://github.com/cucumber/gherkin-c) library
* Thanks to [Cucumber Open Source](https://cucumber.io/) for [cucumber-cpp](https://github.com/cucumber/cucumber-cpp) library
