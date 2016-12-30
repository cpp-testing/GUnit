//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GTest.h"

struct interface {
  virtual ~interface() = default;
  virtual int get(int) const = 0;
  virtual void foo(int) const = 0;
  virtual void bar(int, const std::string&) const = 0;
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

// auto[sut, mocks] = make<T>();
// using Test = testing::GTest<example>;
struct Test : testing::GTest<example> {
  // Test() : testing::GTest<example>(uninitialized{}) {}
  // Test() : testing::GTest<example>(2, 3) {}
};

TEST_F(Test, ShoudlMake) {
  using namespace testing;
  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

TEST_F(Test, ShoudlMakeCustom) {
  using namespace testing;
  std::tie(sut, mocks) = make<example>(/*2, 1*/);
}

TEST_F(Test, ShoudlMakeCpp17) {
  // auto [sut, mocks] = make<example>();
  //...
}
