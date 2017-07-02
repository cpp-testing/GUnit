#language: en

Feature: Test table

  Scenario: Table
    Given I have following table
      | id | desc   |
      | 42 | number |
      | 11 | text |
     When I choose id <id>
     Then I should get <desc>

    Examples:
      | id | desc |
      | 42 | number |
      | 11 | text |
