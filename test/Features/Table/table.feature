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
    Given I access table with 1 row, 1 col
        | id | value  |
        | 42 | number |

  Scenario Outline: Table Ambiguous
    Given I access table with 1 row, 1 col # comment
        | id | value  |
        | 42 | number |
      And I access table with <n>
        | value |
        | 42    |

  Examples:
    | n  |
    | 42 |


  Scenario: Conversions
    Given I have a table
      |  value |
      |   true |
      |  false |
      |      1 |
      |      0 |
      |   TRUE |
      |  FALSE |
    When I do conversion to boolean
    Then I should get
      | value |
      |     1 |
      |     0 |
      |     1 |
      |     0 |
      |     1 |
      |     0 |
