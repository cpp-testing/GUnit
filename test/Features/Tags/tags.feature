@tag
Feature: Tags

  Scenario: No tags
    Given I have a tag
     When I select a test with that tag
     Then I should see test being run

  @wip
  Scenario: Tag
    Given I have a tag
     When I select a test with that tag
     Then I should see test being run

  @wip @slow
  Scenario: Tags
    Given I have a tag
     When I select a test with that tag
     Then I should see test being run

  @wip
  @line
  Scenario: Tags lines
    Given I have a tag
     When I select a test with that tag
     Then I should see test being run

  @disabled
  Scenario: Disabled
    Given I have a tag
     When I select a test with that tag
     Then I should see test being run

  @slow
  Scenario Outline: Outline tags
    Given I have a tag
     When I select a test with that tag
     Then I should see <name> being run

  Examples:
    | name |
    | test |
    | code |
