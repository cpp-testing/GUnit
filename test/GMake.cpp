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

struct interface {
  virtual ~interface() = default;
  virtual int get(int) const = 0;
  virtual void foo(int) const = 0;
  virtual void bar(int, const std::string&) const = 0;
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

struct interface_dtor {
  virtual int get(int) const = 0;
  virtual ~interface_dtor() {}
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

TEST(GMock, ShouldMakeComplexExampleUsingMakeUniquePtr) {
  using namespace testing;
  auto csp = std::make_shared<GMock<interface>>();
  auto sp = std::make_shared<GMock<interface2>>();
  auto ptr = GMock<interface4>();
  auto ref = GMock<interface_dtor>();

  auto sut = make<std::unique_ptr<complex_example>>(csp, sp, &ptr, ref);
  EXPECT_TRUE(nullptr != sut.get());
}

TEST(GMock, ShouldMakeComplexExampleUsingMakeSharedPtr) {
  using namespace testing;
  auto csp = std::make_shared<GMock<interface>>();
  auto sp = std::make_shared<GMock<interface2>>();
  auto ptr = GMock<interface4>();
  auto ref = GMock<interface_dtor>();

  auto sut = make<std::shared_ptr<complex_example>>(csp, sp, &ptr, ref);
  EXPECT_TRUE(nullptr != sut.get());
}
