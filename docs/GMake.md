## GUnit.GMake

* **Removes boilerplate mocks declaration**
* **Creates System Under Test (SUT) the same way despite the constructor changes**
* Synopsis
  ```cpp
  namespace testing {
    /**
     * [Proposal - generic factories]
     *   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0338r0.pdf
     *
     * @tparam T type to be created ex. std::unique_ptr<example>
     * @tparam TMock mock type
     *          NaggyMock  - warning (default)
     *          StrictMock - error
     *          NiceMock   - ignore
     *
     * @tparam TMocks specific mocks different than defaulted by TMock
     *          ex. StrictGMock<interface>
     *
     * @param args arguments (converts GMock for given types T*, T&, shared_ptr, unique_ptr)
     * @return pair{T, mocks}
     */
    template <class T, template <class> class TMock, class... TMocks, class... TArgs>
    auto make(TArgs&&... args);

    template <class T>
    class GTest : public Test {
    public:
      using SUT = std::unique_ptr<T>;

    protected:
      template <class TMock>
      decltype(auto) mock();

      SUT sut;
      mocks_t mocks;
    };
  } // testing
  ```

## GUnit.GMake - Tutorial by example

### Test (V3 - C++17)

```cpp
TEST(Test, ShouldPrintTextWhenUpdate) {
  using namespace testing;
  auto [sut, mocks] = make<example, NaggyMock>(); // create NaggyMocks when required

  EXPECT_CALL(mocks.mock<iconfig>(), (is_dumpable)()).WillOnce(Return(true));
  EXPECT_CALL(mocks.mock<iprinter>(), (print)("text"));

  sut->update();
}
```
* (+) Required mocks are created automatically
* (+) `example` constructor might be refactored without changing test cases!

  ```cpp
  make<example, NaggyMock>() can create...
    example(const std::shared_ptr<iprinter>& printer, const iconfig& config);
    example(const iconfig& config, iprinter& printer);
    example(iconfig* config, iprinter* printer);
    ...
  ```

### Test (V3 - C++14)

```cpp
TEST(Test, ShouldPrintTextWhenUpdate) {
  using namespace testing;
  std::unique_ptr<example> sut;
  mocks_t mocks;
  std::tie(sut, mocks) = make<std::unique_ptr<example>, StrictMock>(); // create StrictMock when required

  EXPECT_CALL(mocks.mock<iconfig>(), (is_dumpable)()).WillOnce(Return(true));
  EXPECT_CALL(mocks.mock<iprinter>(), (print)("text"));

  sut->update();
}
```

> Let's refactor (remove duplicates) from V3 then!

### Test (V4) / using GUnit.GMake

```cpp
class Test : public testing::Test {
public:
  void SetUp() override {
    std::tie(sut, mocks) =
      testing::make<std::unique_ptr<example>, NaggyMock>();
  }
};
```

```cpp
TEST(Test, ShouldPrintTextWhenUpdate) {
  using namespace testing;
  EXPECT_CALL(mock<iconfig>(), (is_dumpable)()).WillOnce(Return(true));
  EXPECT_CALL(mock<iprinter>(), (print)("text"));

  sut->update();
}
```

* (+) **No repetitions with more than 1 test!**
