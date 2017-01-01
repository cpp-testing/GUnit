#GUnit

<a href="http://www.boost.org/LICENSE_1_0.txt" target="_blank">![Boost Licence](http://img.shields.io/badge/license-boost-blue.svg)</a>
<a href="https://travis-ci.org/cpp-testing/GUnit" target="_blank">![Build Status](https://img.shields.io/travis/cpp-testing/GUnit/master.svg?label=linux/osx)</a>

##Towards Painless TDD...

Tested compilers:
  * Clang-3.5+
  * GCC-5+

* HOW
  * it's not standard
  * vtable layout

* TDD
  * protyping
  * quick feedback
  * refactor

* Simple case

| Before | With GUnit.GMock

* Compile time

* Generic Factories

```cpp
template <class T, class... TArgs>
struct ifactory {
  virtual T create(TArgs...) = 0;
  virtual ~ifactory() = default;
};
```

```cpp
template<class, class...>
class MockFactory;

template<class T>
class MockFactory<T> : public ifactory<T> {
  MOCK_METHOD0_T(create, T());
};
template<class T, class T1>
class MockFactory<T, T1> : public ifactory<T, T1> {
  MOCK_METHOD1_T(create, T(T1));
};
template<class T, class T1, class T2>
class MockFactory<T, T1, T2> : public ifactory<T, T1, T2> {
  MOCK_METHOD2_T(create, T(T1, T2));
};
// ...
```

```cpp
class example {
public:
  example(interface1*, const std::shared_ptr<interface2>&, const interface3&);
  void update();
};
```

V1

```cpp
TEST(Test, ShouldCallWhenUpdate) {
  using namespace testing;

  GMock<interface1> interface1_mock;
  auto interface2_mock = std::make_shared<GMock<interface2>>();
  GMock<interface3> interface3_mock;
  auto sut = example{&static_cast<interface1&>(interface1_mock)
                   , std::static_pointer_cast<interface2>(
                   , static_cast<const interface3&>(interface3_mock)};

  EXPECT_CALL(interface1_mock, (f1)()).Times(1);
  EXPECT_CALL(*interface2_mock, (f2)()).Times(1);
  EXPECT_CALL(interface3_mock, (f3)()).Times(1);

  sut.update();
}
```

V2

```cpp
TEST(Test, ShouldCallWhenUpdate) {
  using namespace testing;

  GMock<interface1> interface1_mock;
  auto interface2_mock = std::make_shared<GMock<interface2>>();
  GMock<interface3> interface3_mock;
  auto sut = make<example>(&interface1_mock, interface2_mock, interface3_mock);

  EXPECT_CALL(interface1_mock, (f1)()).Times(1);
  EXPECT_CALL(*interface2_mock, (f2)()).Times(1);
  EXPECT_CALL(interface3_mock, (f3)()).Times(1);

  sut.update();
}
```

V2.1

```cpp
  auto sut = make<std::unique_ptr<example>>(&interface1_mock, interface2_mock, interface3_mock);

  EXPECT_CALL(interface1_mock, (f1)()).Times(1);
  EXPECT_CALL(*interface2_mock, (f2)()).Times(1);
  EXPECT_CALL(interface3_mock, (f3)()).Times(1);

  sut->update();
```

V3 (C++17)

```cpp
TEST(Test, ShouldCallWhenUpdate) {
  using namespace testing;
  auto [sut, mocks] = make<example, NaggyMock>();

  EXPECT_CALL(mocks.mock<interface1>(), (f1)()).Times(1);
  EXPECT_CALL(mocks.mock<interface2>(), (f2)()).Times(1);
  EXPECT_CALL(mocks.mock<interface3>(), (f3)()).Times(1);

  sut.update();
}
```

V4

```cpp
TEST(Test, ShouldCallWhenUpdate) {
  using namespace testing;
  std::unique_ptr<example> sut;
  mocks_t mocks;

  std::tie(sut, mocks) = make<std::unique_ptr<example>, StrictMock>();

  EXPECT_CALL(mocks.mock<interface1>(), (f1)()).Times(1);
  EXPECT_CALL(mocks.mock<interface2>(), (f2)()).Times(1);
  EXPECT_CALL(mocks.mock<interface3>(), (f3)()).Times(1);

  sut->update();
}
```

V5

```cpp
class Test : public GTest {
public:
  Test() {
    std::tie(sut, mocks) = testing::make<std::unique_ptr<example>, NaggyMock>(args...);
  }
};

TEST_F(Test, ShouldCallWhenUpdate) {
  using namespace testing;
  EXPECT_CALL(mock<interface1>(), (f1)()).Times(1);
  EXPECT_CALL(mock<interface2>(), (f2)()).Times(1);
  EXPECT_CALL(mock<interface3>(), (f3)()).Times(1);

  sut->update();
}
```

* Limitations

TODO
uniteresting call - show backtrace
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0338r0.pdf
