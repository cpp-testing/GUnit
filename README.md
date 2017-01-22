<a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a>
<a href="https://travis-ci.org/cpp-testing/GUnit" target="_blank">![Build Status](https://img.shields.io/travis/cpp-testing/GUnit/master.svg?label=linux/osx)</a>
<a href="http://github.com/cpp-testing/GUnit/issues" target="_blank">![Github Issues](https://img.shields.io/github/issues/cpp-testing/GUnit.svg)</a>
[![Join the chat at https://gitter.im/cpp-testing/GUnit](https://badges.gitter.im/cpp-testing/GUnit.svg)](https://gitter.im/cpp-testing/GUnit?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

---

#GUnit
> Towards Painless Testing with GoogleTest and GoogleMock...

![GUnit](https://upload.wikimedia.org/wikipedia/commons/0/0a/G-Unit_logo.png)

>

#Testing

> "If you liked it then you should have put a test on it", Beyonce rule

#TDD (Test Driven Development)

> "If you want to like it then you should have put a test on it FIRST", Beyonce TDD rule

* Red-Green-Refactor requires a quick feedback loop (productivity)
* Goal -> **10 minutes per test case**
* The longer the feedback loop is the more shortcuts will be taken
    * Add a new interface or extend an existing one and break [S]OLID?
    * Refactor/extract an interface or keep it as it is and don't fix [S]OLID?

---

#[GoogleTest](https://github.com/google/googletest)
* (+) Widely used
* (+) Stable
* (+) Powerful
* (+) **Comes with GoogleMock**
* (+) Well documented
* (-) Macro based
* (-) Slow to compile

#[GoogleMock](https://github.com/google/googletest)
* (+) Widely used
* (+) Stable
* (+) Powerful
* (+) Well documented
* (-) Hand written mocks
  * Who likes writing and maintaining these?
  ```cpp
  class MockInterface : public interface {
  public:
    MOCK_CONST_METHOD1(get, bool());
    MOCK_METHOD1(set, void(bool));
  };
  ```
* (-) Macro based
* (-) Slow to compile

#ShowCase/Motivation (Towards Painless Testing)

###Example

```cpp
class interface1 {                          class interface2 {
 public:                                     public:
  virtual ~interface1() = default;             virtual void f2_1() = 0;
  virtual bool f1(int) const = 0;              virtual void f2_2() = 0;
};                                             virtual void f2_3() = 0;
class interface3 {                             virtual void f2_4() = 0;
 public:                                       virtual ~interface2() noexcept {}
  virtual void f3(int, int, int) = 0;        };
  virtual ~interface3() noexcept = default;
};

class example {
 public:
  example(const interface1& i1, interface2& i2, interface3& i3);

  void test() {
    if (i1.f1(42)) {
      i2.f2_1();
    } else {
      i2.f2_2();
    }
    i3.f3(0, 1, 2);
  }

 private:
  const interface1& i1;
  interface2& i2;
  interface3& i3;
};
```

###Test
```cpp
GoogleTest/GoogleMock                           | GUnit
------------------------------------------------+---------------------------------------------
#include <gmock/gmock.h>                        | #include <GUnit.h> // one header
#include <gtest/gtest.h>                        |
                                                |
class mock_i1 : public i1 {                     | // mock_i1 is NOT NEEDED!
public:                                         |
 MOCK_CONST_METHOD1(f1, bool(int));             |
};                                              |
                                                |
class mock_i2 : public i2 {                     | // mock_i2 is NOT NEEDED!
public:                                         |
 MOCK_METHOD0(f2_1, void());                    |
 MOCK_METHOD0(f2_2, void());                    |
 MOCK_METHOD0(f2_3, void());                    |
 MOCK_METHOD0(f2_4, void());                    |
};                                              |
                                                |
class mock_i1 : public i1 {                     | // mock_i3 is NOT NEEDED!
public:                                         |
 MOCK_METHOD3(f3, void(int, int, int));         |
};                                              |
```

```cpp
struct BenchmarkTest : testing::Test {          |
 void SetUp() override {                        |
   sut = std::make_unique<example>(m1, m2, m3); |
 }                                              |
                                                | // setup is NOT NEEDED!
 mock_i1 m1;                                    |
 mock_i2 m2;                                    |
 mock_i3 m3;                                    |
 std::unique_ptr<example> sut;                  |
};                                              |
```

```cpp
TEST_F(BenchmarkTest, ShouldCallF1) {           |GTEST(example) {
 using namespace testing;                       | using namespace testing;
                                                |
 EXPECT_CALL(m1,f1(_)).WillOnce(Return(true));  | SHOULD("call f1") {
 EXPECT_CALL(m2,f2_1()).Times(1);               |  EXPECT_CALL(mock<i1>(),(f1)(_)).WillOnce(Return(true));
 EXPECT_CALL(m3,f3(0, 1, 2)).Times(1);          |  EXPECT_CALL(mock<i2>(),(f2_1)()).Times(1);
                                                |  EXPECT_CALL(mock<i3>(),(f3)(0, 1, 2)).Times(1);
 sut->test();                                   |
}                                               |  sut->test(); // sut and mocks created automatically
                                                | }
TEST_F(BenchmarkTest, ShouldCallF2) {           |
 using namespace testing;                       | SHOULD("call f2") {
                                                |  EXPECT_CALL(mock<i1>(),(f1)(_)).WillOnce(Return(false));
 EXPECT_CALL(m1,f1(_)).WillOnce(Return(false)); |  EXPECT_CALL(mock<i2>(),(f2_2)()).Times(1);
 EXPECT_CALL(m2,f2_2()).Times(1);               |  EXPECT_CALL(mock<i3>(),(f3)(0, 1, 2)).Times(1);
 EXPECT_CALL(m3,f3(0, 1, 2)).Times(1);          |
                                                |  sut->test();
 sut->test();                                   | }
}                                               |}
```

> Note: GUnit deduces constructor parameters automatically!

#GUnit
* Header only library
* Based on top of GoogleTest/GoogleMock
  * `GUnit.GMock` - GoogleMock without hand written mocks
  * `GUnit.GTest` - Makes creation of System Under Test (SUT) easier
* Requirements
  * [C++14](https://ubershmekel.github.io/cppdrafts/c++14-cd.html)
  * [GoogleTest](https://github.com/google/googletest) (compatible with all versions)
  * Tested compilers
    * [Clang-3.6+ / GCC-5+](https://travis-ci.org/cpp-testing/GUnit)
* Quick start
  ```sh
  $mkdir build && cd build && cmake ..
  $make && ctest
  ```
* Goals
  * Makes TDD feedback loop quicker by...
    * **Eliminating hand written mocks**
    * **Speeding up compilation times**
    * **Simplifying System Under Test (SUT) creation**
* How?
  * It's not a C++ standard solution (depends on vtable implementation)
    * [Itanium C++ ABI (vtable)](https://mentorembedded.github.io/cxx-abi/abi.html)

    ![GNU/Clang](http://img.my.csdn.net/uploads/201101/15/4457637_1295058284pWf7.jpg)

    * [VTable in GCC](http://stackoverflow.com/questions/6258559/what-is-the-vtt-for-a-class)
    * [Deleting Destructors](http://eli.thegreenplace.net/2015/c-deleting-destructors-and-virtual-operator-delete)
    * [Devirtualization in C++](http://hubicka.blogspot.com/2014/01/devirtualization-in-c-part-1.html)
    * [Member Function Pointers and the Fastest Possible C++ Delegates](https://www.codeproject.com/kb/cpp/fastdelegate.aspx)
    * ```g++ -fdump-class-hierarchy interface.hpp```

  * Similar projects ([FakeIt](https://github.com/eranpeer/FakeIt), [HippoMocks](https://github.com/dascandy/hippomocks))
* Compile time [benchmark (Example)](https://github.com/cpp-testing/GUnit/tree/master/benchmark)

    | Compiler | Number of Mocks | GoogleMock/GoogleTest | GoogleMock/GoogleTest + GUnit |
    | -------- | --------------- | --------------------- | ----------------------------- |
    | GCC-6    |               3 |                  2.6s |                         2.1s  |
    | Clang-3.9|               3 |                  2.3s |                         1.9s  |

#GUnit.GMock
 * **GoogleMock without writing and maintaining mocks by hand**
 * Supported features
   * `EXPECT_CALL` (requires additional parens for function call)
   ```cpp
   EXPECT_CALL(mock, function()).WillOnce(Return(true)); // GoogleMock
   EXPECT_CALL(mock, (function)()).WillOnce(Return(true)); // GUnit.GMock
   ```
   * `ON_CALL` (requires additional parens for function call)
   ```cpp
   ON_CALL(mock, function()).WillByDefault(Return(true)); // GoogleMock
   ON_CALL(mock, (function)()).WillByDefault(Return(true)); // GUnit.GMock
   ```
   * `Return/ReturnRef`
   * `WaggyMock/StrictMock/NiceMock`
   * Compile error when parameters and expectations don't match
   * It works together with traditional GoogleMock mocks (See [Example](https://github.com/cpp-testing/GUnit/blob/master/test/GMock.cpp))
* Synopsis
  ```cpp
  namespace testing {
    template <class T>
    class GMock {
      static_assert(std::is_abstract<T>::value, "T has to be an abstract type");
      static_assert(std::has_virtual_destructor<T>::value, "T has to have a virtual destructor");

      GMock() = default;
      GMock(GMock &&) = default;
      GMock(const GMock &) = delete;

    public:
      using type = T;

      T&() object();
      const T&() object() const;

      explicit operator T&();
      explicit operator const T&() const;
    };

    template <class T>
    using NaggyGMock = GMock<T>;

    template <class T>
    using StrictGMock = StrictMock<GMock<T>>;

    template <class T>
    using NiceGMock = NiceMock<GMock<T>>;

    /**
     * [Proposal - generic factories]
     *   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0338r0.pdf
     *
     * @tparam T type to be created ex. std::unique_ptr<example>
     * @param args arguments (converts GMock for given types T*, T&, shared_ptr, unique_ptr)
     * @return T instance
     */
    template <class T, class... TArgs>
    auto make(TArgs&&... args);
  } // testing

  namespace std {
    template <class T, class TDeleter>
    std::unique_ptr<T, TDeleter> move(unique_ptr<testing::GMock<T>, TDeleter>& mock) noexcept;

    template <class T, class U>
    std::shared_ptr<U> static_pointer_cast(const std::shared_ptr<testing::GMock<U>>& mock) noexcept;
  } // std
  ```

#GUnit.GMock - Tutorial by Example

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

##Test (V1)
```cpp
TEST(Test, ShouldPrintTextWhenUpdate) {
  using namespace testing;
  GMock<iconfig> mockconfig; // defines and creates a mock
  auto mockprinter = std::make_shared<GMock<iprinter>>(); // defines and creates a mock

  example sut{static_cast<const iconfig&>(mockconfig)
            , std::static_pointer_cast<iprinter>(mockprinter)};

  EXPECT_CALL(mockconfig, (is_dumpable)()).WillOnce(Return(true)); // additional parens
  EXPECT_CALL(*mockprinter, (print)("text")); // additional parens

  sut.update();
}
```

* (+) **NO HAND WRITTEN MOCKS**
* (-) Additional casting of mocks is required
* (~) Additional parens with a method call

> Can we do better?

##Test (V2) / using make

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

* (+) No castings required

> Is the make call generic?

##Test (V2.1) / using make and unique_ptr

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

---

#GUnit.GTest

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

##Test (V3 - C++17)

```cpp
TEST(Test, ShouldPrintTextWhenUpdate) {
  using namespace testing;
  auto [sut, mocks] = make<example, NaggyMock>(); // create NaggyMocks when required

  EXPECT_CALL(mocks.mock<iconfig>(), (is_dumpable)()).WillOnce(Return(true));
  EXPECT_CALL(mocks.mock<iprinter>(), (print)("text"));

  sut.update();
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

##Test (V3 - C++14)

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

##Test (V4) / using GTest

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

* (+) **No repetitions with more than 1 test!**

---

## [Advanced] Constructors with non-interface parameters and make (Assisted Injection)

```cpp
  example(iconfig& config, int value, const std::shared_ptr<iprinter>& printer, int data);
                            ^                                                   ^
                            \_____________________       _______________________/
                                                  \     /
  std::tie(sut, mocks) = make<example, StrictMock>(42, 77); // order of the same types is important
                                                            // but it's not imortant for unique types
```

##[Advanced] Generic Factories

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

* **(+) No specfic factory mocks for given number of parmaeters**
* (+) Factory aliases can be used to determine the mock

---

##Vision for C++20?

* SG7: Mocks generation using static reflection (http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0194r0.pdf)

###Test (V5)
```cpp
"example"_test_fixture = [] {
  auto [sut, mocks] = make<example, NaggyMock>();

  "should print text when update"_test = [=] {
    using namespace testing;
    EXPECT_CALL(mocks<iconfig>(), (is_dumpable)()).WillOnce(Return(true));
    EXPECT_CALL(mocks<iprinter>(), (print)("text"));
    sut->update();
  };
};
```

##Limitations

* GMock can't mock classes with multiple or virtual inheritance
* GMock, by default, can fake interface with up to 128 virtual methods

##FAQ

* How to mock overloaded methods?

  ```cpp
  class interface {
   public:
    virtual void f(int) = 0;
    virtual void f(int) const = 0;
    virtual ~interface() = default;
  };

  GMock<interface> mock;

  EXPECT_CALL(mock, (f, void(int) const)(1));
  EXPECT_CALL(mock, (f, void(int))(2));

  static_cast<const interface&>(mock).f(1);
  mock.object().f(2);
  ```

* Can GUnit be used with [Catch](https://github.com/philsquared/Catch)?
  * Yes, GUnit isn't tied to GoogleTest, however it's tied to GoogleMock

* But virtual function call has performance overhead?
  * This statement is not really true anymore with modern compilers as most virtual calls might be inlined
    * [Devirtualization in C++](http://hubicka.blogspot.co.uk/2014/01/devirtualization-in-c-part-2-low-level.html)
    * [Using final - C++11](https://godbolt.org/g/ASLk4B)
    * [Link Time Optimization - LTO](http://hubicka.blogspot.co.uk/2014/04/linktime-optimization-in-gcc-1-brief.html)

##Acknowledgements
* Thanks to Eran Pe'er and Peter Bindels for [FakeIt](https://github.com/eranpeer/FakeIt) and [HippoMocks](https://github.com/dascandy/hippomocks)

##References
* [Meeting C++ 2016: Mocking C++](https://www.youtube.com/watch?v=6N92PkBeIHw)
* [Meeting C++ 2016: TDD/BDD and Dependency Injection](https://www.youtube.com/watch?v=T3uMcxhzRUE) | [Slides](http://boost-experimental.github.io/di/meetingcpp-2016)
* [Automatic Mocks Injector](https://github.com/cpp-testing/mocks_injector)
* [[Boost].DI](https://github.com/boost-experimental/di) - Dependency Injection Library
