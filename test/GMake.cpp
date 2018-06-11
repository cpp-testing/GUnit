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

class interface3 {
 public:
  virtual ~interface3() = default;
  virtual bool get() const = 0;
};

class extended_interface3 : public interface3 {
 public:
  virtual void foo(bool) = 0;
  virtual void bar(bool) = 0;
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

struct polymorphic_type {
  virtual void foo1() {}
  virtual bool foo2(int) { return true; }
  virtual int bar() const = 0;
  virtual ~polymorphic_type() = default;
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

class example2 {
 public:
  example2(const interface3& i, extended_interface3& ei) : i(i), ei(ei) {}

  void update() {
    const auto value = i.get();
    if (value) {
      ei.foo(value);
    } else {
      ei.bar(value);
    }
  }

 private:
  const interface3& i;
  extended_interface3& ei;
};

class complex_example {
 public:
  complex_example(const std::shared_ptr<interface>& csp,
                  std::shared_ptr<interface2> sp, interface4* ptr,
                  interface_dtor& ref)
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
      : i1(std::move(i1)), i2(std::move(i2)) {}

 private:
  std::unique_ptr<interface> i1;
  std::unique_ptr<interface2> i2;
};

struct polymorphic_example {
  polymorphic_example(std::shared_ptr<interface> i1,
                      const std::shared_ptr<interface2>& i2,
                      polymorphic_type* i3)
      : i1(i1), i2(i2), i3(i3) {}

  std::shared_ptr<interface> i1;
  std::shared_ptr<interface2> i2;
  polymorphic_type* i3 = nullptr;
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
  make<up_example>(std::make_unique<GMock<interface>>(),
                   std::make_unique<GMock<interface2>>());
}

TEST(GMake, ShouldMakeUsingAutoMocksInjection) {
  using namespace testing;
  mocks_t mocks;
  std::unique_ptr<up_example> sut;
  std::tie(sut, mocks) = make<std::unique_ptr<up_example>, StrictGMock>();
  EXPECT_TRUE(sut.get());
  EXPECT_EQ(2u, mocks.size());
}

#if defined(__cpp_structured_bindings)
TEST(GMake, ShouldMakeUsingAutoMocksInjectionStructureBindingsUniquePtr) {
  using namespace testing;
  auto[sut, mocks] = make<std::unique_ptr<polymorphic_example>, StrictGMock>();
  EXPECT_TRUE(sut.get());
  EXPECT_EQ(3u, mocks.size());

  EXPECT_CALL(mocks.mock<polymorphic_type>(), (bar)());
  sut->i3->bar();
}

TEST(GMake, ShouldMakeUsingAutoMocksInjectionStructureBindings) {
  using namespace testing;

  auto[sut, mocks] = make<example2, StrictGMock>();

  EXPECT_CALL(mocks.mock<interface3>(), (get)()).WillOnce(Return(false));
  EXPECT_CALL(mocks.mock<extended_interface3>(), (bar)(false)).Times(1);

  sut.update();
}
#endif

TEST(GMake, ShouldMakePolymorphicTypeUsingAutoMocksInjection) {
  using namespace testing;
  mocks_t mocks;
  std::unique_ptr<polymorphic_example> sut;
  std::tie(sut, mocks) =
      make<std::unique_ptr<polymorphic_example>, StrictGMock>();
  EXPECT_TRUE(sut.get());
  EXPECT_EQ(3u, mocks.size());

  EXPECT_CALL(mocks.mock<polymorphic_type>(), (bar)());
  sut->i3->bar();
}

struct by_value {
  by_value(int) {}
};

TEST(GMake, ShouldMakeAndTryByValueIfRefIsNotProvided) {
  using namespace testing;
  mocks_t mocks;
  std::unique_ptr<by_value> sut;
  int i = 42;
  std::tie(sut, mocks) = make<std::unique_ptr<by_value>, StrictGMock>(i);
  EXPECT_TRUE(sut.get());
  EXPECT_EQ(0u, mocks.size());
}

TEST(GMake, ShouldMakeAndTryByValueConstIfRefIsNotProvided) {
  using namespace testing;
  mocks_t mocks;
  std::unique_ptr<by_value> sut;
  const int i = 42;
  std::tie(sut, mocks) = make<std::unique_ptr<by_value>, StrictGMock>(i);
  EXPECT_TRUE(sut.get());
  EXPECT_EQ(0u, mocks.size());
}

// clang-format off
#if __has_include(<boost/di.hpp>)
// clang-format on
TEST(GMake, ShouldCreateExampleUsingInjector) {
  using namespace testing;
  namespace di = boost::di;
  mocks_t mocks;

  // clang-format off
  const auto injector = di::make_injector(
    di::bind<interface>.to(di::GMock{mocks}) [di::override]
  , di::bind<interface2>.to(di::StrictGMock{mocks}) [di::override]
  );
  // clang-format on

  auto sut = make<std::unique_ptr<example>>(injector);

  EXPECT_TRUE(sut.get());
  EXPECT_EQ(2u, mocks.size());
}

class di_complex_example {
 public:
  di_complex_example(const std::shared_ptr<interface>& csp,
                     std::shared_ptr<interface2> sp, const interface4& cref,
                     interface_dtor& ref)
      : csp(csp), sp(sp), cref(cref), ref(ref) {}

 private:
  [[maybe_unused]] std::shared_ptr<interface> csp;
  [[maybe_unused]] std::shared_ptr<interface2> sp;
  [[maybe_unused]] const interface4& cref;
  [[maybe_unused]] interface_dtor& ref;
};

TEST(GMake, ShouldCreateComplexExampleUsingInjector) {
  using namespace testing;
  namespace di = boost::di;
  mocks_t mocks;

  // clang-format off
  const auto injector = di::make_injector(
     di::bind<interface>.to(di::GMock{mocks}) [di::override]
   , di::bind<interface2>.to(di::StrictGMock{mocks}) [di::override]
   , di::bind<interface4>.to(di::StrictGMock{mocks}) [di::override]
   , di::bind<interface_dtor>.to(di::StrictGMock{mocks}) [di::override]
  );
  // clang-format on

  auto sut = make<std::unique_ptr<di_complex_example>>(injector);

  EXPECT_TRUE(sut.get());
  EXPECT_EQ(4u, mocks.size());
}
#endif
