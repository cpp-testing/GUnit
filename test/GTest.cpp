//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GTest.h"

TEST(GTest, ShouldCompareTypeId) {
  using namespace testing::detail;
  EXPECT_TRUE(type_id<int>() == type_id<int>());
  EXPECT_TRUE(type_id<const int>() == type_id<int>());
  EXPECT_TRUE(type_id<volatile int>() == type_id<int>());
  EXPECT_TRUE(type_id<const int&>() == type_id<const int&>());
  EXPECT_FALSE(type_id<const int&>() == type_id<int&>());
  EXPECT_FALSE(type_id<volatile int* const>() == type_id<int>());
  EXPECT_FALSE(type_id<int&>() == type_id<int>());
  EXPECT_FALSE(type_id<char>() == type_id<int>());
}

TEST(GTest, ShouldReturnTrueWhenTupleContainsType) {
  using namespace testing::detail;
  static_assert(!contains<int, std::tuple<>>::value, "");
  static_assert(!contains<int&, std::tuple<>>::value, "");
  static_assert(!contains<int, std::tuple<float, double>>::value, "");
  static_assert(contains<int, std::tuple<int, double>>::value, "");
  static_assert(contains<int, std::tuple<int, double, int>>::value, "");
}

TEST(GTest, ShouldReturnCtorSize) {
  using namespace testing::detail;

  /*  {*/
  // struct c { c() {} };
  // static_assert(0 == ctor_size<c>::value, "");
  //}

  //{
  // struct c { c(int&) {} };
  // static_assert(1 == ctor_size<c>::value, "");
  /*}*/

  {
    struct c {
      c(int&, int*) {}
    };
    static_assert(2 == ctor_size<c>::value, "");
  }

  {
    struct c {
      c(int, int&, int) {}
    };
    static_assert(3 == ctor_size<c>::value, "");
  }
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

using Test = testing::GTest<example>;

TEST_F(Test, ShouldMakeExample) {
  using namespace testing;
  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

TEST_F(Test, ShouldOverrideSutAndMocks) {
  using namespace testing;
  std::tie(sut, mocks) = Make<example>(123);
  EXPECT_EQ(123, sut->get_data());

  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

using UninitializedTest = testing::GTest<example>;

TEST_F(UninitializedTest, ShoudlNotCreateSUTAndMocks) {
  using namespace testing;

  std::tie(sut, mocks) = Make<example>();

  ASSERT_TRUE(nullptr != sut.get());
  EXPECT_EQ(1u, mocks.size());

  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

struct CtorTest : testing::GTest<example> {
  CtorTest() { std::tie(sut, mocks) = testing::Make<example>(77); }
};

TEST_F(CtorTest, ShoudlPassValueIntoExampleCtor) {
  using namespace testing;

  ASSERT_TRUE(nullptr != sut.get());
  EXPECT_EQ(1u, mocks.size());
  EXPECT_EQ(77, sut->get_data());

  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

struct CtorMultipleTest : testing::GTest<example_data> {
  CtorMultipleTest() { std::tie(sut, mocks) = testing::Make<example_data>(77, 22); }
};

TEST_F(CtorMultipleTest, ShoudlPassMultipleValuesIntoExampleCtor) {
  using namespace testing;

  ASSERT_TRUE(nullptr != sut.get());
  EXPECT_EQ(1u, mocks.size());
  EXPECT_EQ(77, sut->get_data1());
  EXPECT_EQ(22, sut->get_data2());

  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

struct CtorMultiplePlusRefTest : testing::GTest<example_data_ref> {
  CtorMultiplePlusRefTest() { std::tie(sut, mocks) = testing::Make<example_data_ref>(77, i, 22); }
  int i = 42;
};

TEST_F(CtorMultiplePlusRefTest, ShoudlPassMultipleValuesIntoExampleCtor) {
  using namespace testing;

  ASSERT_TRUE(nullptr != sut.get());
  EXPECT_EQ(1u, mocks.size());
  EXPECT_EQ(77, sut->get_data1());
  EXPECT_EQ(22, sut->get_data2());
  EXPECT_EQ(i, sut->get_ref());
  EXPECT_EQ(&i, &sut->get_ref());

  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}
