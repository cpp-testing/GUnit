Feature: Repeat
  Scenario: Repeating steps
  Given I have a text
  | id |
  | 0  |
  When I print it
  Then I should see
  | id |
  | 1  |
  And I have a text
  | id |
  | 2  |
  When I print it
  Then I should see
  | id |
  | 3  |
