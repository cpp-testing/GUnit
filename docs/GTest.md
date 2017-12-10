## GUnit.GTest

* **Simplifies usage of GoogleTest (no more label as test case names!)**

* Synopsis
  ```cpp
    #define GTEST(type_to_be_tested OR test_case_name,
                  [optional] additional_test_case_name,
                  [optional] parametric test values);
    #define DISABLED_GTEST(...); // disable test

    #define SHOULD(test_case_name); creates a new test case inside GTEST
    #define DISABLED_SHOULD(test_case_name); // disable should clause (test case)
  ```

## GUnit.GTest - Tutorial by example

> Simple test
```cpp
GoogleTest                                      | GUnit
------------------------------------------------+---------------------------------------------
TEST(SimpleTest, ShouldDoNothing)               | GTEST("Should do nothing")
{ }                                             | { }
```

> Simple test with a fixture
```cpp
GoogleTest                                      | GUnit
------------------------------------------------+---------------------------------------------
TEST(SimpleTest, ShouldDoNothing)               | GTEST("Simple Test", "Should do nothing")
{ }                                             | { }
```

> Test with a base class
```cpp
GoogleTest                                      | GUnit
------------------------------------------------+---------------------------------------------
struct FooTest : testing::Test { };             | struct FooTest : testing::Test { };
                                                |
TEST_F(FooTest, ShouldDoNothing)                | GTEST(FooTest, "Should do nothing")
{ }                                             | { }
```

> Multiple tests with a base class
```cpp
GoogleTest                                      | GUnit
------------------------------------------------+---------------------------------------------
struct FooTest : testing::Test { };             | struct FooTest : testing::Test { };
                                                |
TEST_F(FooTest, ShouldDoNothingTest1) { }       | GTEST(FooTest, "Should do nothing test 1") { }
TEST_F(FooTest, ShouldDoNothingTest2) { }       | GTEST(FooTest, "Should do nothing test 2") { }
```

> Test with SUT/Mocks creation
```cpp
GoogleTest                                      | GUnit
------------------------------------------------+---------------------------------------------
class IFoo;                                     | class IFoo;
class Example;                                  | class Example;
                                                |
TEST(FooTest, ShouldCallFoo) {                  | GTEST(Example, "Should call foo") {
  std::shared_ptr<StrictGMock<IFoo>> fooMock    |   EXPECT_CALL(mock<IFoo>(), (foo)())
   = std::make_shared<StrictGMock<IFoo>>();     |     .WillOnce(Return(42));
                                                |   EXPECT_EQ(42, sut->run());
  std::unique_ptr<Example> sut                  | }
   = std::make_unique<Example>(object(fooMock));|
                                                |
  EXPECT_CALL(*fooMock, (foo)())                |
    .WillOnce(Return(42));                      |
  EXPECT_EQ(42, sut->run());                    |
}                                               |
```

> Multiple tests with SUT and Mocks
```cpp
GoogleTest                                      | GUnit
------------------------------------------------+---------------------------------------------
class IFoo;                                     | class IFoo;
class Example;                                  | class Example;
                                                |
struct FooTest : testing::Test {                | GTEST(Example) {
  std::shared_ptr<StrictGMock<IFoo>> fooMock    |   std::cout << "set up" << '\n';
   = std::make_shared<StrictGMock<IFoo>>();     |
  std::unique_ptr<Example> sut                  |   SHOULD("call foo") {
   = std::make_unique<Example>(object(fooMock));|     EXPECT_CALL(mock<IFoo>(), (foo)())
                                                |       .WillOnce(Return(42));
  void SetUp() override {                       |     EXPECT_EQ(42, sut->run());
    std::cout << "set up" << '\n';              |   }
  }                                             |
                                                |   SHOULD("call foo and return 0") {
  void TearDown() override {                    |     EXPECT_CALL(mock<IFoo>(), (foo)())
    std::cout << "tear down" << '\n';           |       .WillOnce(Return(0));
  }                                             |     EXPECT_EQ(0, sut->run());
};                                              |   }
                                                |
TEST_F(FooTest, ShouldCallFoo) {                |   std::cout << "tear down" << '\n';
  EXPECT_CALL(*fooMock, (foo)())                | }
    .WillOnce(Return(42));                      |
  EXPECT_EQ(42, sut->run());                    | // There are 2 tests cases here!
}                                               | //   1.	Example.Should call foo
                                                | //   2. Example.Should call foo and return 0
TEST_F(FooTest, ShouldCallFooAndRet0) {         | //
  EXPECT_CALL(*fooMock, (foo)())                | // SetUp and TeardDown will be called
    .WillOnce(Return(0));                       | // separately for both of them
  EXPECT_EQ(0, sut->run());                     |
}
```

> Disable simple test
```cpp
GoogleTest                                      | GUnit
------------------------------------------------+---------------------------------------------
TEST(DISABLED_Test, ShouldDoSomething)          | DISABLED_GTEST("Should do something")
{ }                                             | { }
```

> Disable multiple tests
```cpp
GoogleTest                                      | GUnit
------------------------------------------------+---------------------------------------------
TEST_F(FooTest, DISABLED_ShouldDoA) {}          | DISABLED_GTEST(FooTest) {
TEST_F(FooTest, DISABLED_ShouldDoB) {}          |   SHOULD("Do A") {}
                                                |   SHOULD("Do B") {}
                                                | }
```

> Disable some tests
```cpp
GoogleTest                                      | GUnit
------------------------------------------------+---------------------------------------------
TEST_F(FooTest, ShouldDoA) {}                   | GTEST(FooTest) {
TEST_F(FooTest, DISABLED_ShouldDoB) {}          |   DISABLED_SHOULD("Do A") {}
```                                             |   SHOULD("Do B") {}
                                                | }
```

> Parametrized tests
```cpp
GoogleTest                                      | GUnit
------------------------------------------------+---------------------------------------------
class ParamTest :                               | GTEST("ParamTest", "[InstantiationName]",
  public ::testing::TestWithParam<int> { };     |       testing::Values(1, 2, 3)) {
                                                |  SHOULD("be true") { EXPECT_TRUE(GetParam() >= 1; }
TEST_P(ParamTest, ShouldbeTrue) {               |  SHOULD("be false") { EXPECT_FALSE(false); }
  EXPECT_TRUE(GetParam() >= 1);                 | }
}                                               |
                                                |
TEST_P(ParamTest, ShouldBeFalse) {              |
  EXPECT_FALSE(false);                          |
}                                               |
                                                |
INSTANTIATE_TEST_CASE_P(                        |
  InstantiationName,                            |
  ParamTest,                                    |
  testing::Values(1, 2, 3)                      |
);                                              |
 ```

> Note Running specific `should` test case requires ':' in the test filter (`--gtest_filter="test case pattern:should pattern"`)

*  --gtest_filter="FooTest*:Do A"  # calls FooTest with should("Do A")
*  --gtest_filter="FooTest*:-Do A" # calls FooTest with not should("Do A")
*  --gtest_filter="FooTest*:Do*"   # calls FooTest with should("Do...")
*  --gtest_filter="FooTest.:Do*"   # calls FooTest with should("Do...")
*  --gtest_filter="-FooTest?:-Do*" # calls not FooTest with not should("Do...")

#### Example output

```sh
[----------] 1 tests from Example
[ RUN      ] Example.Return
[ SHOULD   ] return true
[ SHOULD   ] return false
[       OK ] Example.Return (0 ms)
[----------] 1 tests from Example (0 ms total)
```
