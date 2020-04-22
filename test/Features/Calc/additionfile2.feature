# language: en
# When a same Feature is defined in more than one feature files and the Scenarios are at the same line number
# then the gunit should be able to handle it. In this file Scenarios are defined at line# 16, 17 and 18. 
# In addition.feature file also the scenarios are defined at line# 16, 17, 18 and 19.

Feature: Calc Addition
  Scenario Outline: Add two numbers, file 2
    Given I created a calculator with initial value equals 0
    And I have entered <input_1> into the calculator
    And I have entered <input_2> into the calculator
    When I press <button>
    Then the result should be <output> on the screen

  Examples:
    | input_1 | input_2 | button | output |
    | 2       | 3       | add    | 5      |
    | 20      | 50      | add    | 70     |
    | 10      | 40      | add    | 50     |
