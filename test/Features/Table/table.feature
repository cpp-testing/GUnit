Feature: Table

  Scenario Outline: Table
    Given I have the following table
      | id | desc   |
      | 42 | number |
      | 11 | text   |
     When I choose <id>
     Then I should get '<desc>'

     Examples:
       | id | desc |
       | 42 | number |
       | 11 | text |

  Scenario: Table Access
    Given I access table with 1 row
        | id | value  |
        | 42 | number |

  Scenario Outline: Table Ambigious
    Given I access table with 1 row # comment
        | id | value  |
        | 42 | number |
      And I access table with <n>
        | value |
        | 42    |

  Examples:
    | n  |
    | 42 |
