//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GTest.h"

struct interface {
  virtual ~interface() = default;
  virtual void foo(int) const = 0;
  virtual void bar(int, const std::string&) const = 0;
};

class example {
 public:
  example(int data, interface& i) : data(data), i(i) {}

  void update() {
    i.foo(42);
    i.bar(1, "str");
  }

  auto get_data() const { return data; }

 private:
  int data = {};
  interface& i;
};

////////////////////////////////////////////////////////////////////////////////

class GTest : public testing::GTest<example> {
 public:
  GTest() {
    using namespace testing;
    std::tie(sut, mocks) = make<SUT, NaggyGMock>(42);
  }
};

TEST_F(GTest, ShouldMakeExample) {
  using namespace testing;
  EXPECT_EQ(42, sut->get_data());

  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

TEST_F(GTest, ShouldOverrideExample) {
  using namespace testing;

  std::tie(sut, mocks) = make<SUT, NaggyGMock>(77);
  EXPECT_EQ(77, sut->get_data());

  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}
