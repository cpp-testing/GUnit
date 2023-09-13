## GUnit.GSteps - BDD (Given/When/Then - Gherkin) scenarios

* Gherkin reference
  * https://github.com/cucumber/cucumber/wiki/Gherkin

* Building libgherkin-cpp
  ```
  $cd libs/gherkin-cpp && make lib
  $ls libs/gherkin-cpp
    libgherkin-cpp.a
    libgherkin-cpp.so
  ```

* Synopsis
  ```cpp
  /**
   * @param feature.scenario regex expression matching a feature name
   *        example: "Calc*" - all scenarios from Calc feature
   *        example: "Calc Addition.Add two numbers" - Only 'Add two numbers' scenario from Calc feature
   */
  #define GSTEPS(name) // register steps for a feature.scenario

  namespace testing {
    /**
     * Thrown when implementation for given step can't be found
     */
    struct StepIsNotImplemented : std::runtime_error;

    /**
     * Thrown when more than one implementation for given step was be found
     */
    struct StepIsAmbiguous : std::runtime_error;

    /**
     * Verify whether parameters provided in the step and lambda expression
     * matches at compile-time
     */
    constexpr auto operator""_step();

    class convertible {
     public:
      /**
       * Implicit conversion to any type
       * @return converted type
       */
      template <class T> operator T() const;

      /**
       * Verifies whether a field was set
       *
       * @return true if field is available, false otherwise
       */
      bool available() const;
    };

    class table {
     public:
       /**
        * Multiline data text
        */
       std::string text{};
    };

    /**
     * Table parameters from the scenario
     */
    using Table = table<unordered_map<string, convertible>>;
    using Data = Table;
  } // testing

  class GSTEPS {
    /**
     * @param pattern step description (support simple regex)
     *        might be followed by _step to verify parameters at compile time
     * @param table optional table parameter, lambda expression will need a Table parameter
     *
     * Lambda expression has to be assigned
     */
    auto Given(auto pattern, auto table = none);

    /**
     * Same as Given but it will show file/line from cpp files instead of feature file
     */
    auto $Given(auto pattern, auto table = none);

    auto When(auto pattern, auto table = none);
    auto $When(auto pattern, auto table = none);

    auto Then(auto pattern, auto table = none);
    auto $Then(auto pattern, auto table = none);

    struct Info {
      std::string file{};
      std::string feature{};
      std::string scenario{};
      std::string step{};
    };

    /**
     * Returns information about currently being run scenario
     */
    auto& Info() const;
  }
  ```

* Usage

```cpp
/**
 * @param args default-constructible types to be injected
 */
GSTEPS("*") { // * - any feature
  Given("Step...") = [] { /* action */ };
   When("Step...") = [] { /* action */ };
   Then("Step...") = [] { /* action */ };
}
```

* test/Features/Calc/addition.feature
```gherkin
Feature: Calc Addition
  In order to avoid silly mistakes
  As a math idiot
  I want to be told the sum of two numbers

  Scenario Outline: Add two numbers
    Given I created a calculator with initial value equals 0
    And I have entered <input_1> into the calculator
    And I have entered <input_2> into the calculator
    When I press <button>
    Then the result should be <output> on the screen

  Examples:
    | input_1 | input_2 | button | output |
    | 20      | 30      | add    | 50     |
    | 2       | 5       | add    | 7      |
    | 0       | 40      | add    | 40     |
    | 3       | 222     | add    | 225    |
```

#### Steps Implementation
```cpp
GSTEPS("Calc*") {
  using namespace testing;
  double result{};

  Given("I created a calculator with initial value equals {n}"_step) =
    [&](double n) {
      Calculator calc{n};

      Given("I have entered {n} into the calculator") =
        [&](double n) {
          calc.push(n);
        };

      When("I press add") =
        [&] {
          result = calc.add();
        };

      When("I press divide") =
        [&] {
          result = calc.divide();
        };

      Then("the result should be {expected} on the screen") =
        [&](double expected) {
          EXPECT_EQ(expected, result);
        };
    };
}
```

#### Usage
```sh
SCENARIO="test/Features/Calc/addition.feature" ./test --gtest_filter="Calc Addition.Add two numbers"
```

##### Cucumber.json output

GSteps also supports the output of Cucumber.json files
enable this by setting the following environment variables:

```sh
OUTPUT_CUCUMBER_JSON=<your output location>
TEST_NAME=<your test name>
```
the OUTPUT_CUCUMBER_JSON variable can be set in the CMakePreset.json file.
and the test name will be automatically set by the test runner if you define the tests using the `test()` function in the CMakeLists.txt file and use the ctest command to run your tests.

##### Example

###### Define tests
The CMake preset file test preset:
```json
"testPresets": [
        {
            "name": "gcc",
            "configurePreset": "gcc",
            "output": {
                "outputOnFailure": true
            },
            "execution": {
                "noTestsAction": "error",
                "stopOnFailure": false
            },
            "environment": 
            {
                "OUTPUT_CUCUMBER_JSON": "${sourceDir}/TestResults/"
            }
        }
]
```

The CMakeLists.txt file defines a test executable with multiple scenarios:
```
test( test/Features/Calc/Steps/CalcSteps SCENARIO=${CMAKE_CURRENT_SOURCE_DIR}/test/Features/Calc/addition.feature:${CMAKE_CURRENT_SOURCE_DIR}/test/Features/Calc/additionfile2.feature)
```

The test runner will automatically set the TEST_NAME environment variable to the name of the test executable.

###### Build and run tests
```sh
cmake --build -j --preset=gcc
ctest --preset=gcc
```

### GWT and Mocking?

```cpp
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
```

> Note Running specific `scenario` requires ':' in the test filter (`--gtest_filter="feature.scenario"`)

*  --gtest_filter="Calc Addition.Add two numbers"  # calls Calc features test using Addition feature and Add two numbers scenario

* More examples
   * [Features](https://github.com/cpp-testing/GUnit/tree/master/test/Features)
