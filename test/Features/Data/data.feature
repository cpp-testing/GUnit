Feature: Data
  Scenario: Text
    Given I have a multiline text
     """
     first line
     second line
     third line
     """
   When I print it
   Then I should see
     """
     first line
     second line
     third line
     """
