Feature: Example
  In order to show off GSteps
  As a new user
  I want to be told how to use it

  Scenario: Showing off
    Given I create steps
      And I add step a
      And I add step b
     When I run the scenario
     Then The following steps should be shown on the screen
      | step |
      | a    |
      | b    |
