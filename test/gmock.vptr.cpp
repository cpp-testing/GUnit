//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "gmock.vptr.h"
#include <stdexcept>
#include "gtest/gtest.h"

struct interface {
  virtual ~interface() = default;
  virtual int get(int) const = 0;
  virtual void foo(int) const = 0;
  virtual void bar(int, const std::string&) const = 0;
};

struct interface_dtor {
  virtual int get(int) const = 0;
  virtual ~interface_dtor() {}
};

struct interface_gmock {
  virtual ~interface_gmock() = default;
  virtual void f(int) const = 0;
};

struct gmock_interface : interface_gmock {
  MOCK_CONST_METHOD1(f, void(int));
};

class gmock_example {
 public:
  gmock_example(interface_gmock& ig, interface* i) : ig(ig), i(i) {}

  void test() { ig.f(i->get(42)); }

 private:
  interface_gmock& ig;
  interface* i = nullptr;
};

struct interface2 : interface {
  virtual int f1(double) = 0;
};

struct arg {
  int data = {};

  bool operator==(const arg& rhs) const { return data == rhs.data; }
};

struct interface4 : interface {
  virtual void f2(arg) = 0;
};

struct interface3 : interface4 {
  virtual void empty() const = 0;
};

template <class T, class... TArgs>
struct ifactory {
  virtual T create(TArgs...) = 0;
  virtual ~ifactory() = default;
};

class example {
 public:
  example(int, interface& i) : i(i) {}

  void update() {
    i.foo(42);
    i.bar(1, "str");
  }

 private:
  interface& i;
};

class cexample final {
 public:
  explicit cexample(const interface& i) : i(i) {}

  void update() { i.bar(1, "str"); }

 private:
  const interface& i;
};

class cpexample {
 public:
  cpexample(const interface* i) : i(i) {}

  void update() { i->bar(1, "str"); }

 private:
  const interface* i = nullptr;
};

class upexample {
 public:
  explicit upexample(std::unique_ptr<interface> i) : i(std::move(i)) {}

  void update() { i->bar(1, "str"); }

 private:
  std::unique_ptr<interface> i;
};

class throw_example {
 public:
  throw_example(interface& i) : i(i) {}

  void update() {
    if (trigger) {
      throw std::runtime_error{"error"};
    } else {
      i.foo(42);
    }

    trigger ^= 1;
  }

 private:
  interface& i;
  bool trigger = false;
};

TEST(GMockVptr, ShouldReturnFunctionOffset) {
  using namespace testing;
  EXPECT_EQ(2, detail::vptr_offset(&interface::get));
  EXPECT_EQ(3, detail::vptr_offset(&interface::foo));
  EXPECT_EQ(4, detail::vptr_offset(&interface::bar));
}

TEST(GMockVptr, ShouldBeConvertible) {
  using namespace testing;
  GMock<interface> m;

  {
    interface* i = &m;
    (void)i;
  }
  {
    const interface* i = &m;
    (void)i;
  }
  {
    interface& i = m;
    (void)i;
  }
  {
    const interface& i = m;
    (void)i;
  }
}

TEST(GMockVptr, ShouldMockSimpleInterface) {
  using namespace testing;
  GMock<interface> m;

  EXPECT_CALL(m, (foo)(42)).Times(1);
  EXPECT_CALL(m, (foo)(12)).Times(0);
  EXPECT_CALL(m, (bar)(_, "str"));

  example e{0, m};
  e.update();
}

TEST(GMockVptr, ShouldMockExtendedInterface) {
  using namespace testing;
  GMock<interface2> m;
  interface2* i = &m;

  EXPECT_CALL(m, (foo)(42)).Times(1);
  EXPECT_CALL(m, (bar)(_, "str"));
  EXPECT_CALL(m, (f1)(_)).WillOnce(Return(42));

  i->foo(42);
  i->bar(1, "str");
  EXPECT_EQ(42, i->f1(2.0));
}

TEST(GMockVptr, ShouldMockExtendedInterfaceWithArg) {
  using namespace testing;
  GMock<interface4> m;

  EXPECT_CALL(m, (get)(0)).WillOnce(Return(42));
  EXPECT_CALL(m, (f2)(arg{42})).Times(1);

  interface4& i = m;
  EXPECT_EQ(42, i.get(0));
  i.f2(arg{42});
}

TEST(GMockVptr, ShouldHandleMultipleCalls) {
  using namespace testing;
  GMock<interface> m;
  interface& i = m;

  EXPECT_CALL(m, (get)(42)).Times(1);
  i.get(42);

  EXPECT_CALL(m, (get)(_)).Times(1);
  i.get(12);

  EXPECT_CALL(m, (foo)(42)).Times(1);
  i.foo(42);

  EXPECT_CALL(m, (foo)(_)).Times(1);
  i.foo(12);

  EXPECT_CALL(m, (get)(42)).Times(1);
  i.get(42);

  EXPECT_CALL(m, (foo)(42)).Times(1);
  i.foo(42);
}

TEST(GMockVptr, ShouldMockVariadicFactory) {
  using namespace testing;
  GMock<ifactory<int, short, int>> m;
  ifactory<int, short, int>* i = &m;

  EXPECT_CALL(m, (create)(_, 3)).WillOnce(Return(42));
  EXPECT_EQ(42, i->create(1, 3));
}

TEST(GMockVptr, ShouldMockVariadicFactories) {
  using namespace testing;
  GMock<ifactory<std::string, float>> m;
  ifactory<std::string, float>* i = &m;

  std::string str = "str";
  EXPECT_CALL(m, (create)(8.0)).WillOnce(Return(str));
  EXPECT_STREQ("str", i->create(8.0).c_str());
}

TEST(GMockVptr, ShouldMockUsingUniquePtr) {
  using namespace testing;
  auto m = std::make_unique<GMock<interface>>();

  EXPECT_CALL(*m, (foo)(42)).Times(1);
  EXPECT_CALL(*m, (foo)(12)).Times(0);
  EXPECT_CALL(*m, (bar)(_, "str"));

  example e{0, *m};
  e.update();
}

TEST(GMockVptr, ShouldMockUsingAndPassingUniquePtr) {
  using namespace testing;
  auto m = std::make_unique<GMock<interface>>();
  auto* i = m.get();

  EXPECT_CALL(*i, (bar)(_, "str"));

  upexample e{std::move(m)};
  e.update();
  // uninterested dtor call
}

TEST(GMockVptr, ShouldMockUsingSharedPtr) {
  using namespace testing;
  auto m = std::make_shared<GMock<interface>>();

  EXPECT_CALL(*m, (foo)(42)).Times(1);
  EXPECT_CALL(*m, (foo)(12)).Times(0);
  EXPECT_CALL(*m, (bar)(_, "str"));

  example e{0, *m};
  e.update();
}

TEST(GMockVptr, ShouldMockUsingConstInterface) {
  using namespace testing;
  GMock<interface> m;

  EXPECT_CALL(m, (bar)(_, "str"));

  cexample e{m};
  e.update();
}

TEST(GMockVptr, ShouldMockUsingConstPointer) {
  using namespace testing;
  GMock<interface> m;

  EXPECT_CALL(m, (bar)(_, "str"));

  cpexample e{&m};
  e.update();
}

TEST(GMockVptr, ShouldMockEmptyMethods) {
  using namespace testing;
  GMock<interface3> m;

  EXPECT_CALL(m, (empty)()).Times(1);

  static_cast<const interface3*>(&m)->empty();
}

TEST(GMockVptr, ShouldWorkTogetherWithGMock) {
  using namespace testing;
  gmock_interface gm;
  GMock<interface> m;

  EXPECT_CALL(m, (get)(42)).WillOnce(Return(87));
  EXPECT_CALL(gm, f(87)).Times(1);

  auto e = gmock_example{gm, &m};

  e.test();
}

TEST(GMockVptr, ShouldHandleExceptions) {
  using namespace testing;
  GMock<interface> m;
  EXPECT_CALL(m, (foo)(42)).Times(1);

  throw_example e{m};
  e.update();

  EXPECT_THROW(e.update(), std::runtime_error);
}

TEST(GMockVptr, ShouldHandleInterfaceWithDtorNotBeingAtTheBeginning) {
  using namespace testing;
  StrictGMock<interface_dtor> m;
  EXPECT_CALL(m, (get)(0)).Times(1);
  static_cast<interface_dtor&>(m).get(0);
}

TEST(GMockVptr, ShouldHandleON_CALL) {
  using namespace testing;
  NiceGMock<interface> m;
  ON_CALL(m, (get)(_)).WillByDefault(Return(42));
  EXPECT_EQ(42, static_cast<interface&>(m).get(0));
}

TEST(GMockVptr, ShouldHandleMissingExpectations) {
  using namespace testing;
  NiceMock<GMock<interface_dtor>> m;
  static_cast<interface_dtor&>(m).get(0);
}

// auto[sut, mocks] = make<T>();
// using MyTest = testing::GTest<example>;
struct MyTest : testing::GTest<example> {
  // MyTest() : testing::GTest<example>(uninitialized{}) {}
  // MyTest() : testing::GTest<example>(2, 3) {}
};

TEST_F(MyTest, ShoudlMake) {
  using namespace testing;
  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

TEST_F(MyTest, ShoudlMakeCustom) {
  using namespace testing;
  std::tie(sut, mocks) = make<example>(/*2, 1*/);
}

TEST_F(MyTest, ShoudlMakeCpp17) {
  // auto [sut, mocks] = make<example>();
  //...
}
