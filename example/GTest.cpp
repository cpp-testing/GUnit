//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GTest.h"
#include "GUnit/GMake.h"

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

class example_no_data {
 public:
  explicit example_no_data(interface& i) : i(i) {}

  void update() {
    i.foo(42);
    i.bar(1, "str");
  }

 private:
  interface& i;
};

////////////////////////////////////////////////////////////////////////////////

GTEST("Simple Test", "[True/False should be True/False]") {
  EXPECT_TRUE(true);
  EXPECT_FALSE(false);
}

GTEST("Vector test") {
  std::vector<int> sut{};
  EXPECT_TRUE(sut.empty());

  SHOULD("increase the size after a push back") {
    sut.push_back(42);
    EXPECT_EQ(1u, sut.size());
  }

  SHOULD("increase the size after a emplace back") {
    sut.emplace_back(42);
    EXPECT_EQ(1u, sut.size());
  }

  DISABLED_SHOULD("disabled should") {}
  SHOULD("do nothing") {}
}

class VectorTest : public testing::Test {
 protected:
  void SetUp() override { sut.push_back(42); }
  std::vector<int> sut;
};

GTEST(VectorTest, "[Using Test]") {
  EXPECT_EQ(1u, sut.size());  // from VectorTest::SetUp
  sut.push_back(77);          // SetUp

  SHOULD("increase the size after a emplace back") {
    EXPECT_EQ(2u, sut.size());
    sut.push_back(21);
    EXPECT_EQ(3u, sut.size());
  }
}

GTEST(example) {
  // SetUp - will be run for each SHOULD section and it will create sut and
  // mocks if possible
  using namespace testing;
  std::tie(sut, mocks) = make<SUT, StrictGMock>(42);

  SHOULD("make simple example") {
    EXPECT_EQ(42, sut->get_data());

    EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
    EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

    sut->update();
  }

  SHOULD("override example") {
    std::tie(sut, mocks) = make<SUT, NaggyGMock>(77);
    EXPECT_EQ(77, sut->get_data());

    EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
    EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

    sut->update();
  }

  // TearDown
}

GTEST(example_no_data) {
  // SetUp - will be run for each SHOULD section and it will create sut and
  // mocks if possible
  using namespace testing;

  SHOULD("make simple example") {
    EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
    EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

    sut->update();
  }

  // TearDown
}
