//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GTest.h"

TEST(GTest, ShouldReturnTrueWhenTupleContainsType) {
  static_assert(!testing::detail::contains<int, std::tuple<>>::value, "");
  static_assert(!testing::detail::contains<int&, std::tuple<>>::value, "");
  static_assert(!testing::detail::contains<int, std::tuple<float, double>>::value, "");
  static_assert(testing::detail::contains<int, std::tuple<int, double>>::value, "");
  static_assert(testing::detail::contains<int, std::tuple<int, double, int>>::value, "");
}

struct interface {
  virtual ~interface() = default;
  virtual int get(int) const = 0;
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

class example_data {
 public:
  example_data(int data1, interface& i, int data2) : data1(data1), i(i), data2(data2) {}

  void update() {
    i.foo(42);
    i.bar(1, "str");
  }

  auto get_data1() const { return data1; }
  auto get_data2() const { return data2; }

 private:
  int data1 = {};
  interface& i;
  int data2 = {};
};

class example_data_ref {
 public:
  example_data_ref(int data1, interface& i, int& ref, int data2) : data1(data1), i(i), ref(ref), data2(data2) {}

  void update() {
    i.foo(42);
    i.bar(1, "str");
  }

  auto get_data1() const { return data1; }
  auto get_data2() const { return data2; }
  decltype(auto) get_ref() const { return ref; }

 private:
  int data1 = {};
  interface& i;
  int& ref;
  int data2 = {};
};

// using Test = testing::GTest<example>;

// TEST_F(Test, ShoudlMake) {
// using namespace testing;
// EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
// EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

// sut->update();
//}

// TEST_F(Test, ShouldOverrideSutAndMocks) {
// using namespace testing;
// std::tie(sut, mocks) = make<example>([>2, 1<]);

// EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
// EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

// sut->update();
//}

// using UninitializedTest = testing::GTest<example, testing::uninitialized>;

// TEST_F(UninitializedTest, ShoudlNotCreateSUTAndMocks) {
// using namespace testing;

// EXPECT_TRUE(nullptr == sut.get());
// EXPECT_TRUE(mocks.empty());

// std::tie(sut, mocks) = make<example>();

// ASSERT_TRUE(nullptr != sut.get());
// EXPECT_EQ(1u, mocks.size());

// EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
// EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

// sut->update();
//}

// struct CtorTest : testing::GTest<example> {
// CtorTest() : { initialize(77)  }
//};

// TEST_F(CtorTest, ShoudlPassValueIntoExampleCtor) {
// using namespace testing;

// ASSERT_TRUE(nullptr != sut.get());
// EXPECT_EQ(1u, mocks.size());
// EXPECT_EQ(77, sut->get_data());

// EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
// EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

// sut->update();
//}

// struct CtorMultipleTest : testing::GTest<example_data> {
// CtorMultipleTest() { initialize(77, 22); }
//};

// TEST_F(CtorMultipleTest, ShoudlPassMultipleValuesIntoExampleCtor) {
// using namespace testing;

// ASSERT_TRUE(nullptr != sut.get());
// EXPECT_EQ(1u, mocks.size());
// EXPECT_EQ(77, sut->get_data1());
// EXPECT_EQ(22, sut->get_data2());

// EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
// EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

// sut->update();
//}

struct CtorMultiplePlusRefTest : testing::GTest<example_data_ref, testing::uninitialized> {
  CtorMultiplePlusRefTest() { make_<example_data_ref>(77, i, 22); }
  int i = 42;
};

TEST_F(CtorMultiplePlusRefTest, ShoudlPassMultipleValuesIntoExampleCtor) {
  using namespace testing;

  std::tie(sut, mocks) = make<example_data_ref>(77, 22);

  ASSERT_TRUE(nullptr != sut.get());
  EXPECT_EQ(1u, mocks.size());
  EXPECT_EQ(77, sut->get_data1());
  EXPECT_EQ(22, sut->get_data2());
  EXPECT_EQ(i, sut->get_ref());
  // EXPECT_EQ(&i, &sut->get_ref());

  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

// interface unique_ptr, shraed_ptr
// int, blah&, int
