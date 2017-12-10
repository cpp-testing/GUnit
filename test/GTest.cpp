//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GTest.h"
#include <memory>
#include <string>

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

  {
    struct c {
      c() {}
    };
    static_assert(0 == ctor_size<c>::value, "");
  }

  {
    struct c {
      c(int&) {}
    };
    static_assert(1 == ctor_size<c>::value, "");
  }

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

struct interface2 : interface {
  virtual int f1(double) = 0;
  virtual void f2(int) = 0;
};

struct interface_dtor {
  virtual int get(int) const = 0;
  virtual ~interface_dtor() {}
};

struct arg {
  int data = {};
  bool operator==(const arg& rhs) const { return data == rhs.data; }
};

struct interface4 : interface {
  virtual void f2(arg) const = 0;
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
  example_data(int data1, interface& i, const std::string& data2)
      : data1(data1), i(i), data2(data2) {}

  void update() {
    i.foo(42);
    i.bar(1, "str");
  }

  auto get_data1() const { return data1; }
  auto get_data2() const { return data2; }

 private:
  int data1 = {};
  interface& i;
  std::string data2 = {};
};

class example_data_ref {
 public:
  example_data_ref(int data1, interface& i, int& ref, const std::string& data2,
                   const int& cref)
      : data1(data1), i(i), ref(ref), data2(data2), cref(cref) {}

  void update() {
    i.foo(42);
    i.bar(1, "str");
  }

  auto get_data1() const { return data1; }
  auto get_data2() const { return data2; }
  decltype(auto) get_ref() const { return ref; }
  decltype(auto) get_cref() const { return cref; }

 private:
  int data1 = {};
  interface& i;
  int& ref;
  std::string data2 = {};
  const int& cref;
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

class complex_example_const {
 public:
  complex_example_const(const std::shared_ptr<interface>& csp,
                        const std::shared_ptr<interface2>& sp,
                        const interface4* ptr, const interface_dtor& ref)
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
  const interface4* ptr;
  const interface_dtor& ref;
};

class complex_example_const_string {
 public:
  complex_example_const_string(const std::shared_ptr<interface>& csp,
                               const std::shared_ptr<interface2>& sp,
                               const interface4* ptr, const interface_dtor& ref,
                               const std::string& name)
      : csp(csp), sp(sp), ptr(ptr), ref(ref), name(name) {}

  void update() {
    const auto i = csp->get(42);
    sp->f1(77.0);
    ptr->f2(arg{});
    ref.get(i);
  }

  const std::string& get_name() const { return name; }

 private:
  std::shared_ptr<interface> csp;
  std::shared_ptr<interface2> sp;
  const interface4* ptr;
  const interface_dtor& ref;
  std::string name;
};

struct same_mock {
  same_mock(const std::shared_ptr<interface>& csp,
            std::shared_ptr<interface> sp)
      : csp(csp), sp(sp) {}

  void update() {
    csp->get(42);
    sp->bar(1, "str");
  }

  std::shared_ptr<interface> csp;
  std::shared_ptr<interface> sp;
};

class upexample {
 public:
  explicit upexample(std::unique_ptr<interface> i) : i(std::move(i)) {}
  void update() { i->bar(1, "str"); }

 private:
  std::unique_ptr<interface> i;
};

struct non_default_constructible {
  non_default_constructible(int) {}
};

class nexample {
 public:
  explicit nexample(std::unique_ptr<interface> i, non_default_constructible)
      : i(std::move(i)) {}
  void update() { i->bar(1, "str"); }

 private:
  std::unique_ptr<interface> i;
};

class imock_return {
 public:
  virtual const interface& get(bool) const = 0;
  virtual ~imock_return() = default;
};

class imock_return_sp {
 public:
  virtual std::shared_ptr<interface> get(bool) const = 0;
  virtual ~imock_return_sp() = default;
};

class mexample final {
 public:
  explicit mexample(const imock_return& i) : i(i) {}
  void update(const bool value) { i.get(value).foo(42); }

 private:
  const imock_return& i;
};

class mspexample final {
 public:
  explicit mspexample(const std::shared_ptr<imock_return_sp>& i) : i(i) {}
  void update(const bool value) { i->get(value)->foo(42); }

 private:
  std::shared_ptr<imock_return_sp> i;
};

class imock_return_ptr {
 public:
  virtual interface* get(bool) const = 0;
  virtual ~imock_return_ptr() = default;
};

class mptrexample final {
 public:
  explicit mptrexample(const imock_return_ptr* i) : i(i) {}
  void update(const bool value) { i->get(value)->foo(42); }

 private:
  const imock_return_ptr* i;
};

class longest_ctor final {
 public:
  longest_ctor(short s, double d, std::shared_ptr<interface> i)
      : s(s), d(d), i(i) {}

  longest_ctor(short s, double d, std::shared_ptr<interface> i,
               const std::shared_ptr<interface2>& i2)
      : s(s), d(d), i(i), i2(i2) {}

  short s = {};
  double d = {};
  std::shared_ptr<interface> i;
  std::shared_ptr<interface2> i2;
};

class longest_ctor_force final {
 public:
  longest_ctor_force(short s, double d, std::shared_ptr<interface> i, ...)
      : s(s), d(d), i(i) {}

  longest_ctor_force(short s, double d, std::shared_ptr<interface> i,
                     const std::shared_ptr<interface2>& i2)
      : s(s), d(d), i(i), i2(i2) {}

  short s = {};
  double d = {};
  std::shared_ptr<interface> i;
  std::shared_ptr<interface2> i2;
};

class passing_ref {
 public:
  passing_ref(interface*, arg&) {}
};

TEST(MakeTest, ShouldMakeSimpleExampleUsingMakeAndTie) {
  using namespace testing;
  struct c {
    c() = default;
    c(int i, double d, float f) : i(i), d(d), f(f) {}
    int i = {};
    double d = {};
    float f = {};
  } sut;
  mocks_t mocks;

  {
    std::tie(sut, mocks) = make<c, NaggyGMock>(1, 2.f, 3.);
    EXPECT_TRUE(mocks.empty());
    EXPECT_EQ(sut.i, 1);
    EXPECT_DOUBLE_EQ(sut.f, 2.f);
    EXPECT_DOUBLE_EQ(sut.d, 3.);
  }

  {
    std::tie(sut, mocks) = make<c, NaggyGMock>(2.f, 1, 3.);
    EXPECT_TRUE(mocks.empty());
    EXPECT_EQ(sut.i, 1);
    EXPECT_DOUBLE_EQ(sut.f, 2.f);
    EXPECT_DOUBLE_EQ(sut.d, 3.);
  }

  {
    std::tie(sut, mocks) = make<c, NaggyGMock>(3., 1, 2.f);
    EXPECT_TRUE(mocks.empty());
    EXPECT_EQ(sut.i, 1);
    EXPECT_DOUBLE_EQ(sut.f, 2.f);
    EXPECT_DOUBLE_EQ(sut.d, 3.);
  }

  {
    std::tie(sut, mocks) = make<c, StrictGMock>(3., 1, 2.f);
    EXPECT_TRUE(mocks.empty());
    EXPECT_EQ(sut.i, 1);
    EXPECT_DOUBLE_EQ(sut.f, 2.f);
    EXPECT_DOUBLE_EQ(sut.d, 3.);
  }

  {
    std::tie(sut, mocks) = make<c, NiceGMock>(3., 1, 2.f);
    EXPECT_TRUE(mocks.empty());
    EXPECT_EQ(sut.i, 1);
    EXPECT_DOUBLE_EQ(sut.f, 2.f);
    EXPECT_DOUBLE_EQ(sut.d, 3.);
  }
}

TEST(PassingArgTest, ShouldMakePassingRef) {
  using namespace testing;
  mocks_t mocks;
  std::unique_ptr<passing_ref> sut;
  auto a = std::make_unique<arg>();
  std::tie(sut, mocks) =
      make<std::unique_ptr<passing_ref>, testing::NaggyGMock>(*a);
  EXPECT_TRUE(sut.get());
}

TEST(MakeTest, ShouldThrowThenRequestedMockIsNotFound) {
  using namespace testing;
  std::unique_ptr<example> sut;
  mocks_t mocks;
  std::tie(sut, mocks) = make<std::unique_ptr<example>, NiceGMock>(42);

  EXPECT_NO_THROW(mocks.mock<interface>());
  EXPECT_THROW(mocks.mock<interface4>(), mock_exception<interface4>);
}

TEST(MakeTest, ShouldMakeComplexExampleUsingMakeAndTie) {
  using namespace testing;
  std::unique_ptr<complex_example> sut;
  mocks_t mocks;

  std::tie(sut, mocks) =
      make<std::unique_ptr<complex_example>, testing::NaggyGMock>();
  EXPECT_TRUE(nullptr != sut.get());
  ASSERT_EQ(4u, mocks.size());
}

struct Test : testing::GTest<example> {
  void SetUp() override {
    std::tie(sut, mocks) = testing::make<SUT, testing::NaggyGMock>(42);
  }
};

TEST_F(Test, ShouldMakeExample) {
  using namespace testing;
  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

TEST_F(Test, ShouldOverrideSutAndMocks) {
  using namespace testing;
  std::tie(sut, mocks) = make<SUT, NaggyGMock>(123);
  EXPECT_EQ(123, sut->get_data());
  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

using UninitializedTest = testing::GTest<example>;

TEST_F(UninitializedTest, ShouldNotCreateSUTAndMocks) {
  using namespace testing;

  ASSERT_TRUE(nullptr == sut.get());
  EXPECT_TRUE(mocks.empty());

  std::tie(sut, mocks) = make<SUT, NaggyGMock>(42);

  ASSERT_TRUE(nullptr != sut.get());
  EXPECT_EQ(1u, mocks.size());

  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

struct CtorTest : testing::GTest<example> {
  void SetUp() override {
    std::tie(sut, mocks) = testing::make<SUT, testing::NaggyGMock>(77);
  }
};

TEST_F(CtorTest, ShouldPassValueIntoExampleCtor) {
  using namespace testing;

  ASSERT_TRUE(nullptr != sut.get());
  EXPECT_EQ(1u, mocks.size());
  EXPECT_EQ(77, sut->get_data());

  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

struct CtorMultipleTest : testing::GTest<example_data> {
  void SetUp() override {
    std::tie(sut, mocks) =
        testing::make<SUT, testing::NaggyGMock>(77, std::string{"text"});
  }
};

TEST_F(CtorMultipleTest, ShouldPassMultipleValuesIntoExampleCtor) {
  using namespace testing;

  ASSERT_TRUE(nullptr != sut.get());
  EXPECT_EQ(1u, mocks.size());
  EXPECT_EQ(77, sut->get_data1());
  EXPECT_EQ(std::string{"text"}, sut->get_data2());

  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

struct CtorMultiplePlusRefTest : testing::GTest<example_data_ref> {
  void SetUp() override {
    std::tie(sut, mocks) = testing::make<SUT, testing::NaggyGMock>(
        77, ref, std::string{"text"}, cref);
  }
  int ref = 42;
  const int cref = 7;
};

TEST_F(CtorMultiplePlusRefTest, ShouldPassMultipleValuesIntoExampleCtor) {
  using namespace testing;

  ASSERT_TRUE(nullptr != sut.get());
  EXPECT_EQ(1u, mocks.size());
  EXPECT_EQ(77, sut->get_data1());
  EXPECT_EQ(std::string{"text"}, sut->get_data2());
  EXPECT_EQ(ref, sut->get_ref());
  EXPECT_EQ(&ref, &sut->get_ref());
  EXPECT_EQ(cref, sut->get_cref());
  EXPECT_EQ(&cref, &sut->get_cref());

  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

struct CtorMultiplePlusRefOrderTest : testing::GTest<example_data_ref> {
  void SetUp() override {
    std::tie(sut, mocks) = testing::make<SUT, testing::NaggyGMock>(
        cref, 77, ref, std::string{"text"});
  }
  int ref = 42;
  const int cref = 7;
};

TEST_F(CtorMultiplePlusRefOrderTest, ShouldPassMultipleValuesIntoExampleCtor) {
  using namespace testing;

  ASSERT_TRUE(nullptr != sut.get());
  EXPECT_EQ(1u, mocks.size());
  EXPECT_EQ(77, sut->get_data1());
  EXPECT_EQ(std::string{"text"}, sut->get_data2());
  EXPECT_EQ(ref, sut->get_ref());
  EXPECT_EQ(&ref, &sut->get_ref());
  EXPECT_EQ(cref, sut->get_cref());
  EXPECT_EQ(&cref, &sut->get_cref());

  EXPECT_CALL(mock<interface>(), (foo)(42)).Times(1);
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

  sut->update();
}

struct ComplexTest : testing::GTest<complex_example> {
  void SetUp() override {
    std::tie(sut, mocks) = testing::make<SUT, testing::NaggyGMock>();
  }
};

TEST_F(ComplexTest, ShouldMakeComplexExample) {
  using namespace testing;

  EXPECT_CALL(mock<interface>(), (get)(_)).WillOnce(Return(123));
  EXPECT_CALL(mock<interface2>(), (f1)(77.0)).Times(1);
  EXPECT_CALL(mock<interface4>(), (f2)(_)).Times(1);
  EXPECT_CALL(mock<interface_dtor>(), (get)(123)).Times(1);

  sut->update();
}

struct ComplexStrictMocksTest : testing::GTest<complex_example> {};

TEST_F(ComplexStrictMocksTest, ShouldMakeComplexExampleWithStrictMocks) {
  using namespace testing;

  std::tie(sut, mocks) = testing::make<SUT, testing::StrictGMock>();

  EXPECT_CALL(mock<interface>(), (get)(_)).WillOnce(Return(123));
  EXPECT_CALL(mock<interface2>(), (f1)(77.0)).Times(1);
  EXPECT_CALL(mock<interface4>(), (f2)(_)).Times(1);
  EXPECT_CALL(mock<interface_dtor>(), (get)(123)).Times(1);

  sut->update();
}

struct ComplexNiceMocksTest : testing::GTest<complex_example> {};

TEST_F(ComplexNiceMocksTest, ShouldMakeComplexExampleWithNiceMocks) {
  using namespace testing;

  std::tie(sut, mocks) = testing::make<SUT, testing::NiceGMock>();

  EXPECT_CALL(mock<interface>(), (get)(_)).WillOnce(Return(123));
  // missing calls should be nicely ignored by NiceMock

  sut->update();
}

struct ComplexConstTest : testing::GTest<complex_example_const> {
  void SetUp() override {
    std::tie(sut, mocks) = testing::make<SUT, testing::NaggyGMock>();
  }
};

TEST_F(ComplexConstTest, ShouldMakeComplexConstExample) {
  using namespace testing;

  EXPECT_CALL(mock<interface>(), (get)(_)).WillOnce(Return(123));
  EXPECT_CALL(mock<interface2>(), (f1)(77.0)).Times(1);
  EXPECT_CALL(mock<interface4>(), (f2)(_)).Times(1);
  EXPECT_CALL(mock<interface_dtor>(), (get)(123)).Times(1);

  sut->update();
}

struct ComplexWithStringTest : testing::GTest<complex_example_const_string> {
  void SetUp() override {
    std::tie(sut, mocks) =
        testing::make<SUT, testing::NaggyGMock>(std::string{"str"});
  }
};

TEST_F(ComplexWithStringTest, ShouldMakeComplexConstExample) {
  using namespace testing;

  EXPECT_EQ(std::string{"str"}, sut->get_name());
  EXPECT_CALL(mock<interface>(), (get)(_)).WillOnce(Return(123));
  EXPECT_CALL(mock<interface2>(), (f1)(77.0)).Times(1);
  EXPECT_CALL(mock<interface4>(), (f2)(_)).Times(1);
  EXPECT_CALL(mock<interface_dtor>(), (get)(123)).Times(1);

  sut->update();
}

using UniquePtrTest = testing::GTest<upexample>;

TEST_F(UniquePtrTest, ShouldMakeUpExample) {
  using namespace testing;
  std::tie(sut, mocks) = make<SUT, NaggyGMock>();
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));
  sut->update();
}

using UniquePtrAndNonDefaultConstructibleTest = testing::GTest<nexample>;

TEST_F(UniquePtrAndNonDefaultConstructibleTest, ShouldMakeNExample) {
  using namespace testing;
  std::tie(sut, mocks) = make<SUT, NaggyGMock>(non_default_constructible{42});
  EXPECT_CALL(mock<interface>(), (bar)(_, "str"));
  sut->update();
}

struct SameMockTest : testing::GTest<same_mock> {
  void SetUp() override {}
};

GTEST(same_mock) {
  using namespace testing;
  std::tie(sut, mocks) = testing::make<SUT, testing::NaggyGMock>();

  SHOULD("pass the same mock") {
    EXPECT_TRUE(sut->csp.get() == sut->sp.get());
    EXPECT_CALL(mock<interface>(), (get)(_)).WillOnce(Return(123));
    EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

    sut->update();
  }
}

GTEST(mexample) {
  using namespace testing;
  std::tie(sut, mocks) = testing::make<SUT, testing::NaggyGMock>();
  StrictGMock<interface> mockInterface;

  SHOULD("pass GMock type") {
    constexpr auto value = true;
    EXPECT_CALL(mock<imock_return>(), (get)(value))
        .WillOnce(ReturnRef(mockInterface));
    EXPECT_CALL(mockInterface, (foo)(42)).Times(1);

    sut->update(value);
  }
}

GTEST(mspexample) {
  using namespace testing;
  std::tie(sut, mocks) = testing::make<SUT, testing::NaggyGMock>();
  std::shared_ptr<testing::StrictGMock<interface>> mockInterface =
      std::make_shared<testing::StrictGMock<interface>>();

  SHOULD("pass GMock shared ptr type") {
    constexpr auto value = false;
    EXPECT_CALL(mock<imock_return_sp>(), (get)(value))
        .WillOnce(Return(mockInterface));
    EXPECT_CALL(*mockInterface, (foo)(42)).Times(1);

    sut->update(value);
  }
}

GTEST(mptrexample) {
  using namespace testing;
  std::tie(sut, mocks) = testing::make<SUT, testing::NaggyGMock>();
  testing::StrictGMock<interface> mockInterface;

  SHOULD("pass GMock ptr type") {
    constexpr auto value = false;
    EXPECT_CALL(mock<imock_return_ptr>(), (get)(value))
        .WillOnce(Return(&mockInterface));
    EXPECT_CALL(mockInterface, (foo)(42)).Times(1);

    sut->update(value);
  }
}

GTEST(longest_ctor) {
  using namespace testing;
  std::tie(sut, mocks) =
      testing::make<SUT, testing::NaggyGMock>(short{42}, 77.0);

  SHOULD("take the longest ctor") {
    EXPECT_EQ(short{42}, sut->s);
    EXPECT_EQ(77.0, sut->d);
    EXPECT_TRUE(sut->i.get());
    EXPECT_TRUE(sut->i2.get());
  }
}

GTEST(longest_ctor_force) {
  using namespace testing;

  SHOULD("not create sut and mocks") {
    EXPECT_FALSE((detail::is_creatable<longest_ctor_force>::value));
    EXPECT_FALSE(sut.get());
  }

  std::tie(sut, mocks) =
      testing::make<SUT, testing::NaggyGMock>(short{42}, 77.0);

  SHOULD("take injected, longest ctor") {
    EXPECT_EQ(short{42}, sut->s);
    EXPECT_EQ(77.0, sut->d);
    EXPECT_TRUE(sut->i.get());
    EXPECT_FALSE(sut->i2.get());  // not initialized
  }
}

GTEST(complex_example) {
  using namespace testing;

  // setup
  std::tie(sut, mocks) = testing::make<SUT, testing::NiceGMock>();
  EXPECT_CALL(mock<interface>(), (get)(_)).WillOnce(Return(123));
  EXPECT_CALL(mock<interface2>(), (f1)(77.0)).Times(1);
  EXPECT_CALL(mock<interface4>(), (f2)(_)).Times(1);
  // Missing interface_dtor.get expectation

  sut->update();

  SHOULD("create sut and mocks") {
    EXPECT_TRUE((detail::is_creatable<complex_example>::value));
    EXPECT_TRUE(sut.get());
  }

  SHOULD("make complex example with strict mocks") {
    std::tie(sut, mocks) =
        testing::make<SUT, StrictGMock, NiceGMock<interface_dtor>>();
    EXPECT_CALL(mock<interface>(), (get)(_)).WillOnce(Return(123));
    EXPECT_CALL(mock<interface2>(), (f1)(77.0)).Times(1);
    EXPECT_CALL(mock<interface4>(), (f2)(_)).Times(1);
    // Missing interface_dtor.get expectation

    sut->update();
  }

  SHOULD("make complex example with nice mocks") {
    std::tie(sut, mocks) = testing::make<SUT, testing::NiceGMock>();
    EXPECT_CALL(mock<interface>(), (get)(_)).WillOnce(Return(123));
    EXPECT_CALL(mock<interface2>(), (f1)(77.0)).Times(1);
    EXPECT_CALL(mock<interface4>(), (f2)(_)).Times(1);
    // Missing interface_dtor.get expectation

    sut->update();
  }
}

struct is_default_constructible {};

GTEST(is_default_constructible) {
  using namespace testing;

  SHOULD("create a default constructible type") {
    EXPECT_TRUE((detail::is_creatable<is_default_constructible>::value));
    EXPECT_TRUE(sut.get());
  }
}

GTEST(example, "[should create sut with a mock]") {
  using namespace testing;
  ASSERT_TRUE(nullptr == sut.get());
  EXPECT_EQ(0u, mocks.size());

  SHOULD("override sut and pass polymorphic type") {
    StrictGMock<interface> i;
    std::tie(sut, mocks) = make<SUT, StrictGMock>(42, i.object());

    EXPECT_THROW(mock<interface>(), mock_exception<interface>);
    EXPECT_CALL(i, (foo)(42));
    EXPECT_CALL(i, (bar)(_, "str"));

    sut->update();
  }

  SHOULD("override sut and create a mock type") {
    std::tie(sut, mocks) = make<SUT, StrictGMock>(42);

    EXPECT_CALL(mock<interface>(), (foo)(42));
    EXPECT_CALL(mock<interface>(), (bar)(_, "str"));

    sut->update();
  }
}

GTEST("Make with mocks") {
  SHOULD("make complex example with default and custom mocks") {
    using namespace testing;
    using SUT = std::unique_ptr<complex_example>;
    SUT sut;
    mocks_t mocks;

    std::tie(sut, mocks) =
        testing::make<SUT, testing::StrictGMock, NiceGMock<interface_dtor>>();
    std::tie(sut, mocks) =
        testing::make<SUT, testing::NiceGMock, StrictGMock<interface_dtor>>();
    std::tie(sut, mocks) =
        testing::make<SUT, testing::NiceGMock, NaggyGMock<interface_dtor>>();
    std::tie(sut, mocks) =
        testing::make<SUT, testing::NaggyGMock, StrictGMock<interface_dtor>,
                      NiceGMock<interface>>();
  }
}

GTEST("Example Test", "[Example]") {
  SHOULD("expect true") { EXPECT_TRUE(true); }
}

GTEST("Example Test", "[OtherExample]") {
  SHOULD("expect false") { EXPECT_FALSE(false); }
}

class MyTest : public testing::Test {
 protected:
  void SetUp() override { setUpCall = true; }
  void TearDown() override { tearDownCall = true; }

  bool setUpCall = false;
  bool tearDownCall = false;
};

GTEST(MyTest, "[Custom Test]") {
  SHOULD("inherit from MyTest and call setup, teardown") {
    EXPECT_TRUE(setUpCall);
    EXPECT_FALSE(tearDownCall);
  }
}

GTEST("Test without should", "Should Register the test case itself") {
  EXPECT_TRUE(true);
}

GTEST("ParamTest", "[Info]", testing::Values(1, 2, 3)) {
  SHOULD("be true") { EXPECT_TRUE(GetParam() >= 1 && GetParam() <= 3); }
  SHOULD("be false") { EXPECT_FALSE(false); }
  SHOULD("another test") {
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
  }
}

GTEST(example, "[Values]", testing::Values(1, 2, 3)) {
  using namespace testing;
  ASSERT_TRUE(nullptr == sut.get());
  EXPECT_EQ(0u, mocks.size());

  SHOULD("override sut with given param") {
    std::tie(sut, mocks) = make<SUT, StrictGMock>(GetParam());

    EXPECT_INVOKE(mock<interface>(), foo, 42);
    EXPECT_INVOKE(mock<interface>(), bar, _, "str");

    sut->update();

    EXPECT_EQ(GetParam(), sut->get_data());
  }
}

GTEST("Test1") {}
GTEST("Test1", "Desc1") {}
GTEST("Test1", "Desc2") {}
GTEST("Test1", "Desc3") {
  SHOULD("should1") {}
  SHOULD("should2") {}
}

GTEST("Test2") {
  SHOULD("call this one") {}
  SHOULD("call this one but not this one") {}
}

DISABLED_GTEST("TestDisabled1") {}
DISABLED_GTEST("TestDisabled2", "[]") {}
DISABLED_GTEST("TestDisabled3", "[]") {
  SHOULD("a") {}
  SHOULD("b") {}
}

GTEST("TestDisabled4", "[]") {
  DISABLED_SHOULD("a") {}
  SHOULD("b") {}
}

DISABLED_GTEST("TestDisabled5", "[]") {
  DISABLED_SHOULD("a") {}
  DISABLED_SHOULD("b") {}
}

GTEST("TestDisabled6", "[]", testing::Values(1, 2, 3)) {
  SHOULD("a") {}
  DISABLED_SHOULD("b") {}
}

// clang-format off
#if __has_include(<boost/di.hpp>)
// clang-format on
class di_example {
 public:
  di_example(const interface& i1, interface2& i2) : i1(i1), i2(i2) {}

  void update() {
    const auto i = i1.get(42);
    i2.f2(i);
  }

 private:
  const interface& i1;
  interface2& i2;
};

GTEST(di_example) {
  using namespace testing;
  namespace di = boost::di;

  SHOULD("create example") {
    // clang-format off
    const auto injector = di::make_injector(
      di::bind<interface>.to(di::NiceGMock{mocks})
    , di::bind<interface2>.to(di::StrictGMock{mocks})
    );
    // clang-format on

    sut = make<SUT>(injector);

    EXPECT_CALL(mock<interface>(), (get)(_)).WillOnce(Return(123));
    EXPECT_CALL(mock<interface2>(), (f2)(123));

    sut->update();
  }
}

class di_complex_example {
 public:
  di_complex_example(std::shared_ptr<interface> csp,
                     std::shared_ptr<interface2> sp, const interface4& cref,
                     interface_dtor& ref)
      : csp(csp), sp(sp), cref(cref), ref(ref) {}

  void update() {
    const auto i = csp->get(42);
    sp->f1(77.0);
    cref.f2(arg{});
    ref.get(i);
  }

 private:
  std::shared_ptr<interface> csp;
  std::shared_ptr<interface2> sp;
  const interface4& cref;
  interface_dtor& ref;
};

GTEST(di_complex_example, "[Complex Example]") {
  using namespace testing;
  namespace di = boost::di;

  // clang-format off
  const auto injector = di::make_injector(
     di::bind<interface>.to(di::GMock{mocks}) [di::override]
   , di::bind<interface2>.to(di::StrictGMock{mocks}) [di::override]
   , di::bind<interface4>.to(di::StrictGMock{mocks}) [di::override]
   , di::bind<interface_dtor>.to(di::StrictGMock{mocks}) [di::override]
  );
  // clang-format on

  sut = make<SUT>(injector);

  EXPECT_INVOKE(mock<interface>(), get, _).WillOnce(Return(123));
  EXPECT_INVOKE(mock<interface2>(), f1, 77.0).Times(1);
  EXPECT_INVOKE(mock<interface4>(), f2, _).Times(1);
  EXPECT_INVOKE(mock<interface_dtor>(), get, 123).Times(1);

  sut->update();
}
#endif
