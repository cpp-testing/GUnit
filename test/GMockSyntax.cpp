//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#define GUNIT_INVOKE_SYNTAX
#include "GUnit/GMock.h"
#include <gtest/gtest.h>
#include <memory>

struct interface {
  virtual ~interface() = default;
  virtual int get(int) const = 0;
  virtual void foo(int) const = 0;
  virtual void empty() = 0;
  virtual void bar(int, const std::string&) const = 0;
  virtual void overload(int) = 0;
  virtual void overload(double) = 0;
};

class example {
 public:
  example(int, interface& i) : i(i) {}

  void update() {
    i.foo(42);
    i.bar(1, "str");
    i.empty();
  }

 private:
  interface& i;
};

TEST(GMock, ShouldSupportInvokeSyntaxWithExpectCall) {
  using namespace testing;
  auto m = std::make_unique<GMock<interface>>();

  EXPECT_CALL(*m, foo, 42).Times(1);
  EXPECT_CALL(*m, foo, 12).Times(0);
  EXPECT_CALL(*m, bar, _, "str");
  EXPECT_CALL(*m, empty, );

  example sut{0, static_cast<interface&>(*m)};
  sut.update();
}

TEST(GMock, ShouldSupportInvokeSyntaxWithOverloadedCall) {
  using namespace testing;
  GMock<interface> mock;

  EXPECT_CALL(mock, (overload, void(int)), 42);
  mock.object().overload(42);

  EXPECT_CALL(mock, (overload, void(double)), 77.0);
  mock.object().overload(77.0);
}

TEST(GMock, ShouldSupportInvokeSyntaxWithOnCall) {
  using namespace testing;
  NiceGMock<interface> m;
  ON_CALL(m, get, _).WillByDefault(Return(42));
  EXPECT_EQ(42, static_cast<interface&>(m).get(0));
}
