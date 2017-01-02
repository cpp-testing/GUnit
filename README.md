#GUnit

[![Join the chat at https://gitter.im/cpp-testing/GUnit](https://badges.gitter.im/cpp-testing/GUnit.svg)](https://gitter.im/cpp-testing/GUnit?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

<a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a>
<a href="https://travis-ci.org/cpp-testing/GUnit" target="_blank">![Build Status](https://img.shields.io/travis/cpp-testing/GUnit/master.svg?label=linux/osx)</a>

##Towards Painless TDD...

* C++
  - Is great for performance (gives control)
  - Is not as good for productivity in testing

* TDD (Test Driven Development)

  * Red-Green-Refactor requires a quick feedback loop (productivity)
  * Goal -> 10 minutes per test case
  * The longer and harder the feedback is the more shortcus will be taken
      * Example -> Extract an interface

GTest
+ widely used
+ stable
+ powerful
+ come swith GMock
- macro based
- slow to compile

GMock
+ widely used
+ stable
+ powerful
- hand written mocks
  * Who likes writing these?
    ```cpp
      MOCK_CONST_METHOD1(get, bool());
      MOCK_METHOD1(set, void(bool));
    ```
- macro based
- slow to compile

GUnit
* Header only library
* Based on top of GTest/GMock
  * GUnit.GMock - vtable mock
  * GUnit.GTest - Test utilities

Goals:
* Make TDD feedback loop quicker
  * Eliminiate hand written mocks
  * Speed up compilation times
  * Simplyfy SUT creation

Requirements:
  * C++14
  * Clang-3.5+ / GCC-5+
  * GMock/GTest (compatible with all versions)

How
  Not standard (implementation detail of compiler)
  GCC vtable layout example
  Simpilar projects (FakeIt, HippoMocks)

* Compile time benchmark

| GMock | GUnit.GMock |
| ----- | ----------- |
| 0s   | 0s          |

GUnit.GMock
  * Gmock without writing it by hand
  * Supported
    * EXPECT_CALL // requires additional parsns for function call
      ```cpp
      EXPECT_CALL(mock, function()).WillOnce(Return(true)); // GMock
      EXPECT_CALL(mock, (function)()).WillOnce(Return(true)); // GUnit.GMock
      ```
    * ON_CALL // requires additional parsns for function call
      ```cpp
      ON_CALL(mock, function()).WillByDefault(Return(true)); // GMock
      ON_CALL(mock, (function)()).WillByDefault(Return(true)); // GUnit.GMock
      ```
    * WaggyMock/StrictMock/NiceMock
    * Compile error parameters and matches don't match
    * It works together with traditional GMock mocks

* Synoposis

```cpp
template <class T>
class GMock {
  static_assert(std::is_polymorphic<T>::value, "T has to be a polymorphic type");
  static_assert(std::has_virtual_destructor<T>::value, "T has to have a virtual destructor");

public:
  using type = T;
  explicit operator T&();
  explicit operator const T&() const;
};
```

```cpp
template <class T>
using StrictGMock = StrictMock<GMock<T>>;

template <class T>
using NiceGMock = NiceMock<GMock<T>>;
```

GUnit.GTest

```cpp
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
```

```cpp
/**
 * @tparam T type to be created ex. std::unique_ptr<example>
 * @tparam TMock mock type (NoMock - default)
 *          NoMock     - won't create mocks
 *          NaggyMock  - warning (default)
 *          StrictMock - error
 *          NiceMock   - ignore
 *
 * @param args arguments (converts GMock for given types T*, T&, shared_ptr, unique_ptr)
 *
 * @return pair{T, mocks}
 */
template <class T, template <class> class TMock = NoMock, class... TArgs>
auto make(TArgs&&... args);
```
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0338r0.pdf


*Hello World

```cpp
class iconfig {
 public:
  virtual bool is_dumpable() const = 0;
  virtual ~iconfig() = default;
};
```

```cpp
class iprinter {
 public:
  virtual ~iprinter() = default;
  virtual void print(const std::string& text) = 0;
};
```

```cpp
class example {
 public:
  example(const iconfig& config, const std::shraed_ptr<iprinter>& printer)
    : config(config), printer(printer)
  { }

  void update() {
    if (config.is_dumpable()) {
      printer->print("text");
    }
  }

 private:
  const iconfig& config;
  std::shared_ptr<iprinter> printer;
};
```

#Test (V1)

```cpp
TEST(Test, ShouldPrintTextWhenUpdate) {
  using namespace testing;
  GMock<iconfig> mockconfig;
  auto mockprinter = std::make_shared<GMock<iprinter>>();

  example sut{static_cast<const iconfig&>(mockconfig)
            , std::static_pointer_cast<iprinter>(mockprinter)};

  EXPECT_CALL(mockconfig, (is_dumpable)()).WillOnce(Return(true));
  EXPECT_CALL(*mockprinter, (print)("text"));

  sut.update();
}
```

* NO HAND WRITTEN MOCKS
* additional casting of mocks required
* Additional parens with method call

```cpp
EXPECT_CALL(mockconfig, is_dumpable()).WillOnce(Return(true)); // GMock
EXPECT_CALL(mockconfig, (is_dumpable)()).WillOnce(Return(true)); // GUnit.GMock
```

* Can we do better?

#Test (V2)

```cpp
TEST(Test, ShouldPrintTextWhenUpdate) {
  using namespace testing;
  StrictGMock<iconfig> mockconfig; // strict mock
  auto mockprinter = std::make_shared<StrictGMock<iprinter>>(); // strict mock

  auto sut = make<example>(mockconfig, mockprinter); // automatically converts mocks to interfaces

  EXPECT_CALL(mockconfig, (is_dumpable)()).WillOnce(Return(true));
  EXPECT_CALL(*mockprinter, (print)("text"));

  sut.update();
}
```

#Test (V2.1)

```cpp
TEST(Test, ShouldPrintTextWhenUpdate) {
  using namespace testing;
  StrictGMock<iconfig> mockconfig;
  auto mockprinter = std::make_shared<StrictGMock<iprinter>>();

  // create a unique_ptr
  auto sut = make<std::unique_ptr<example>>(mockconfig, mockprinter);

  EXPECT_CALL(mockconfig, (is_dumpable)()).WillOnce(Return(true));
  EXPECT_CALL(*mockprinter, (print)("text"));

  sut->update();
}
```

#Test (V3 - C++17)

```cpp
TEST(Test, ShouldPrintTextWhenUpdate) {
  using namespace testing;
  auto [sut, mocks] = make<example, NaggyMock>(); // create NaggyMocks when required

  EXPECT_CALL(mocks.mock<iconfig>(), (is_dumpable)()).WillOnce(Return(true));
  EXPECT_CALL(mocks.mock<iprinter>(), (print)("text"));

  sut.update();
}
```

* Required mocks are created automatically
* example constructor might be refactored without changing test cases!
  ```cpp
  example(const std::shraed_ptr<iprinter>& printer, const iconfig& config); // parameters order change
  ```

#Test (V3.1 - C++14)

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

#Test (V4)

```cpp
class Test : public GTest<example> {
public:
  void SetUp() override {
    std::tie(sut, mocks) = testing::make<SUT, NaggyMock>();
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

* No repetitions
* Easy to add new tests

* [Advanced] Generic Factories (Assisted Injection)

```cpp
  example(iconfig& config, int value, const std::shared_ptr<iprinter>& printer, int data);
                            ^                                                   ^
                            \_____________________       _______________________/
                                                  \     /
  std::tie(sut, mocks) = make<example, StrictMock>(42, 77); // order of the same types is important
                                                            // but it's not imortant for unique types
```

* [Advanced] Constructors with no-interface parameters

```cpp
template <class T, class... TArgs>
struct ifactory {
  virtual T create(TArgs...) = 0;
  virtual ~ifactory() = default;
};
```

```cpp
using iconfigfactory = ifactory<iconfig, std::string>;
```

```cpp
GMock<iconfig> mockconfig;
EXPECT_CALL(mock<iconfigfactory>(), (create)("string")).WillOnce(Return(mockconfig));
```

---

Vision/Going Forward

Going Forward (BDD)?
* Automatic Mocks Injector

