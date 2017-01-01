//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GMock.h"
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

class spexample {
 public:
  explicit spexample(std::shared_ptr<interface> i) : i(i) {}
  void update() { i->bar(1, "str"); }

 private:
  std::shared_ptr<interface> i;
};

class complex_example {
 public:
  complex_example(const std::shared_ptr<interface>& csp, std::shared_ptr<interface2> sp, interface4* ptr, interface_dtor& ref)
      : csp(csp), sp(sp), ptr(ptr), ref(ref) {}

  void update() {
    const auto i = csp->get(42);
    sp->f1(77.0);
    ptr->f2(arg{});
    ref.get(i);
  }

 private:
  std::shared_ptr<interface> csp;
  std::shared_ptr<interface2> sp;
  interface4* ptr;
  interface_dtor& ref;
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

TEST(GMock, ShouldReturnFunctionOffset) {
  using namespace testing;
  EXPECT_EQ(2u, detail::vf_offset(&interface::get));
  EXPECT_EQ(3u, detail::vf_offset(&interface::foo));
  EXPECT_EQ(4u, detail::vf_offset(&interface::bar));
}

TEST(GMock, ShouldBeConvertible) {
  using namespace testing;
  GMock<interface> m;

  {
    interface* i = &static_cast<interface&>(m);
    (void)i;
  }
  {
    const interface* i = &static_cast<interface&>(m);
    (void)i;
  }
  {
    interface& i = static_cast<interface&>(m);
    (void)i;
  }
  {
    const interface& i = static_cast<interface&>(m);
    (void)i;
  }
}

TEST(GMock, ShouldConstBeConvertible) {
  using namespace testing;
  const GMock<interface> m;

  {
    const interface* i = &static_cast<const interface&>(m);
    (void)i;
  }
  {
    const interface& i = static_cast<const interface&>(m);
    (void)i;
  }
}

TEST(GMock, ShouldMockSimpleInterface) {
  using namespace testing;
  GMock<interface> m;

  EXPECT_CALL(m, (foo)(42)).Times(1);
  EXPECT_CALL(m, (foo)(12)).Times(0);
  EXPECT_CALL(m, (bar)(_, "str"));

  example sut{0, static_cast<interface&>(m)};
  sut.update();
}

TEST(GMock, ShouldMockExtendedInterface) {
  using namespace testing;
  GMock<interface2> m;
  auto* i = &static_cast<interface2&>(m);

  EXPECT_CALL(m, (foo)(42)).Times(1);
  EXPECT_CALL(m, (bar)(_, "str"));
  EXPECT_CALL(m, (f1)(_)).WillOnce(Return(42));

  i->foo(42);
  i->bar(1, "str");
  EXPECT_EQ(42, i->f1(2.0));
}

TEST(GMock, ShouldMockExtendedInterfaceWithArg) {
  using namespace testing;
  GMock<interface4> m;

  EXPECT_CALL(m, (get)(0)).WillOnce(Return(42));
  EXPECT_CALL(m, (f2)(arg{42})).Times(1);

  auto& i = static_cast<interface4&>(m);
  EXPECT_EQ(42, i.get(0));
  i.f2(arg{42});
}

TEST(GMock, ShouldHandleMultipleCalls) {
  using namespace testing;
  GMock<interface> m;
  auto& i = static_cast<interface&>(m);

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

TEST(GMock, ShouldMockVariadicFactory) {
  using namespace testing;
  GMock<ifactory<int, short, int>> m;
  auto* i = &static_cast<ifactory<int, short, int>&>(m);

  EXPECT_CALL(m, (create)(_, 3)).WillOnce(Return(42));
  EXPECT_EQ(42, i->create(1, 3));
}

TEST(GMock, ShouldMockVariadicFactories) {
  using namespace testing;
  GMock<ifactory<std::string, float>> m;
  auto& i = static_cast<ifactory<std::string, float>&>(m);

  std::string str = "str";
  EXPECT_CALL(m, (create)(8.0)).WillOnce(Return(str));
  EXPECT_STREQ("str", i.create(8.0).c_str());
}

TEST(GMock, ShouldMockUsingUniquePtr) {
  using namespace testing;
  auto m = std::make_unique<GMock<interface>>();

  EXPECT_CALL(*m, (foo)(42)).Times(1);
  EXPECT_CALL(*m, (foo)(12)).Times(0);
  EXPECT_CALL(*m, (bar)(_, "str"));

  example sut{0, static_cast<interface&>(*m)};
  sut.update();
}

TEST(GMock, ShouldMockUsingAndPassingUniquePtr) {
  using namespace testing;
  auto m = std::make_unique<GMock<interface>>();
  auto* i = m.get();

  EXPECT_CALL(*i, (bar)(_, "str"));

  upexample sut{std::move(m)};
  sut.update();
}

TEST(GMock, ShouldMockUsingSharedPtr) {
  using namespace testing;
  auto m = std::make_shared<GMock<interface>>();

  EXPECT_CALL(*m, (foo)(42)).Times(1);
  EXPECT_CALL(*m, (foo)(12)).Times(0);
  EXPECT_CALL(*m, (bar)(_, "str"));

  example sut{0, static_cast<interface&>(*m)};
  sut.update();
}

TEST(GMock, ShouldMockUsingAndPassingSharedPtr) {
  using namespace testing;
  auto m = std::make_shared<GMock<interface>>();

  EXPECT_CALL(*m, (bar)(_, "str"));

  auto sut = std::make_unique<spexample>(std::static_pointer_cast<interface>(m));
  sut->update();
}

TEST(GMock, ShouldMockComplexExample) {
  using namespace testing;
  auto csp = std::make_shared<GMock<interface>>();
  auto sp = std::make_shared<GMock<interface2>>();
  GMock<interface4> ptr;
  StrictGMock<interface_dtor> ref;

  EXPECT_CALL(*csp, (get)(_)).WillOnce(Return(123));
  EXPECT_CALL(*sp, (f1)(77.0)).Times(1);
  EXPECT_CALL(ptr, (f2)(_)).Times(1);
  EXPECT_CALL(ref, (get)(123)).Times(1);

  complex_example sut{std::static_pointer_cast<interface>(csp), std::static_pointer_cast<interface2>(sp),
                      &static_cast<interface4&>(ptr), static_cast<interface_dtor&>(ref)};

  sut.update();
}

TEST(GMock, ShouldMockComplexExampleUniquePtr) {
  using namespace testing;
  auto csp = std::make_shared<GMock<interface>>();
  auto sp = std::make_shared<GMock<interface2>>();
  GMock<interface4> ptr;
  StrictGMock<interface_dtor> ref;

  EXPECT_CALL(*csp, (get)(_)).WillOnce(Return(123));
  EXPECT_CALL(*sp, (f1)(77.0)).Times(1);
  EXPECT_CALL(ptr, (f2)(_)).Times(1);
  EXPECT_CALL(ref, (get)(123)).Times(1);

  auto sut =
      std::make_unique<complex_example>(std::static_pointer_cast<interface>(csp), std::static_pointer_cast<interface2>(sp),
                                        &static_cast<interface4&>(ptr), static_cast<interface_dtor&>(ref));
  sut->update();
}

TEST(GMock, ShouldMockUsingConstInterface) {
  using namespace testing;
  GMock<interface> m;

  EXPECT_CALL(m, (bar)(_, "str"));

  cexample sut{static_cast<interface&>(m)};
  sut.update();
}

TEST(GMock, ShouldMockUsingConstPointer) {
  using namespace testing;
  GMock<interface> m;

  EXPECT_CALL(m, (bar)(_, "str"));

  cpexample sut{&static_cast<const interface&>(m)};
  sut.update();
}

TEST(GMock, ShouldMockEmptyMethods) {
  using namespace testing;
  GMock<interface3> m;

  EXPECT_CALL(m, (empty)()).Times(1);

  static_cast<const interface3&>(m).empty();
}

TEST(GMock, ShouldWorkTogetherWithGMock) {
  using namespace testing;
  gmock_interface gm;
  GMock<interface> m;

  EXPECT_CALL(m, (get)(42)).WillOnce(Return(87));
  EXPECT_CALL(gm, f(87)).Times(1);

  auto sut = gmock_example{gm, &static_cast<interface&>(m)};

  sut.test();
}

TEST(GMock, ShouldHandleExceptions) {
  using namespace testing;
  GMock<interface> m;
  EXPECT_CALL(m, (foo)(42)).Times(1);

  throw_example sut{static_cast<interface&>(m)};
  sut.update();

  EXPECT_THROW(sut.update(), std::runtime_error);
}

TEST(GMock, ShouldHandleInterfaceWithDtorNotBeingAtTheBeginning) {
  using namespace testing;
  StrictGMock<interface_dtor> m;
  EXPECT_CALL(m, (get)(0)).Times(1);
  static_cast<interface_dtor&>(m).get(0);
}

TEST(GMock, ShouldHandleON_CALL) {
  using namespace testing;
  NiceGMock<interface> m;
  ON_CALL(m, (get)(_)).WillByDefault(Return(42));
  EXPECT_EQ(42, static_cast<interface&>(m).get(0));
}

TEST(GMock, ShouldHandleMissingExpectations) {
  using namespace testing;
  NiceMock<GMock<interface_dtor>> m;
  static_cast<interface_dtor&>(m).get(0);
}

TEST(GMock, ShouldNotTriggerUnexpectedCallForCtor) {
  using namespace testing;
  std::shared_ptr<void> mock = std::make_shared<GMock<interface>>();
  std::unique_ptr<interface> up = std::unique_ptr<interface>(reinterpret_cast<interface*>(mock.get()));
  (void)up;
}

TEST(GMock, ShouldNotTriggerUnexpectedCallForCtorOrder) {
  using namespace testing;
  std::shared_ptr<void> mock = std::make_shared<GMock<interface_dtor>>();
  std::unique_ptr<interface_dtor> up = std::unique_ptr<interface_dtor>(reinterpret_cast<interface_dtor*>(mock.get()));
  (void)up;
}
