//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GMake.h"
#include <gtest/gtest.h>
#include "GUnit/GMock.h"
#include "GUnit/GTest.h"

struct interface {
  virtual ~interface() = default;
  virtual int get(int) const = 0;
  virtual void foo(int) const = 0;
  virtual void bar(int, const std::string&) const = 0;
};

struct interface2 : interface {
  virtual int f1(double) = 0;
  virtual void f2(int) = 0;
};

struct arg {
  int data = {};
  bool operator==(const arg& rhs) const { return data == rhs.data; }
};

struct interface4 : interface {
  virtual void f2(arg) = 0;
};

struct interface_dtor {
  virtual int get(int) const = 0;
  virtual ~interface_dtor() {}
};

class example {
 public:
  example(const interface& i1, interface2& i2) : i1(i1), i2(i2) {}

  void update() {
    const auto i = i1.get(42);
    i2.f2(i);
  }

 private:
  const interface& i1;
  interface2& i2;
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

TEST(GMake, ShouldMakeComplexExampleUsingMakeUniquePtr) {
  using namespace testing;
  auto csp = std::make_shared<GMock<interface>>();
  auto sp = std::make_shared<GMock<interface2>>();
  auto ptr = GMock<interface4>();
  auto ref = GMock<interface_dtor>();

  auto sut = make<std::unique_ptr<complex_example>>(csp, sp, &ptr, ref);
  EXPECT_TRUE(nullptr != sut.get());
}

TEST(GMake, ShouldMakeComplexExampleUsingMakeSharedPtr) {
  using namespace testing;
  auto csp = std::make_shared<GMock<interface>>();
  auto sp = std::make_shared<GMock<interface2>>();
  auto ptr = GMock<interface4>();
  auto ref = GMock<interface_dtor>();

  auto sut = make<std::shared_ptr<complex_example>>(csp, sp, &ptr, ref);
  EXPECT_TRUE(nullptr != sut.get());
}

TEST(GMake, ShouldMakeComplexExampleUsingMakeType) {
  using namespace testing;
  auto csp = std::make_shared<GMock<interface>>();
  auto sp = std::make_shared<GMock<interface2>>();
  auto ptr = GMock<interface4>();
  auto ref = GMock<interface_dtor>();
  auto sut = make<complex_example>(csp, sp, &ptr, ref);
  (void)sut;
}

class up_example {
 public:
  up_example(std::unique_ptr<interface>&& i1, std::unique_ptr<interface2> i2)
      : i1(std::move(i1)), i2(std::move(i2))
  {}

 private:
  std::unique_ptr<interface> i1;
  std::unique_ptr<interface2> i2;
};

TEST(GMake, ShouldMakeUniquePtrExampleUsingMake) {
  using namespace testing;
  auto up1 = std::make_unique<GMock<interface>>();
  auto up2 = std::make_unique<GMock<interface2>>();
  auto sut = make<up_example>(std::move(up1), std::move(up2));
  (void)sut;
}

TEST(GMake, ShouldMakeUniquePtrInPlaceExampleUsingMake) {
  using namespace testing;
  make<up_example>(std::make_unique<GMock<interface>>(), std::make_unique<GMock<interface2>>());
}

TEST(GMake, ShouldMakeUsingAutoMocksInjection) {
  using namespace testing;
  mocks_t mocks;
  std::unique_ptr<up_example> sut;
  std::tie(sut, mocks) = make<std::unique_ptr<up_example>, StrictGMock>();
  EXPECT_TRUE(sut.get());
  EXPECT_EQ(2u, mocks.size());
}

#if __has_include(<boost / di.hpp>)
TEST(GMake, ShouldCreateUsingInjector) {
  using namespace testing;
  namespace di = boost::di;
  mocks_t mocks;

  // clang-format off
  const auto injector = di::make_injector(
    di::bind<interface>.to(di::GMock{mocks}) [di::override]
  , di::bind<interface2>.to(di::StrictGMock{mocks}) [di::override]
  );
  // clang-format on

  auto object = make<example>(injector);

  EXPECT_CALL(mocks.mock<interface>(), (get)(_)).WillOnce(Return(123));
  EXPECT_CALL(mocks.mock<interface2>(), (f2)(123));

  object.update();
}
#endif
