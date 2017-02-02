//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include "GUnit/Detail/Preprocessor.h"
#include "GUnit/Detail/TypeTraits.h"
#include "GUnit/GMake.h"
#include "GUnit/GMock.h"

#if !defined(GUNIT_SHOULD_PREFIX)
#define GUNIT_SHOULD_PREFIX "should "
#endif

namespace testing {
inline namespace v1 {
namespace detail {

template <class T>
class GTestFactoryImpl final : public internal::TestFactoryBase {
  class TestImpl final : public T {
   public:
    void TestBody() override { T::test(); }
  };

 public:
  Test* CreateTest() override { return new TestImpl{}; }
};

struct TestCaseInfo {
  std::string symbol;
  std::string type;
  std::string name;
  std::string file;
  int line = 0;
  std::string should;

  bool operator==(const TestCaseInfo& ti) const { return symbol == ti.symbol; }
  bool operator!=(const TestCaseInfo& ti) const { return symbol != ti.symbol; }
  bool operator<(const TestCaseInfo& ti) const { return symbol < ti.symbol; }
  bool operator>(const TestCaseInfo& ti) const { return symbol > ti.symbol; }
};

struct TestCaseInfoParser {
  using type = TestCaseInfo;

  static auto parse(const std::string& line) {
    TestCaseInfo ti;
    ti.symbol = line;
    std::string token;
    std::istringstream stream(line);

    const auto parseString = [&](std::string& result) {
      std::getline(stream, token, '<');
      std::getline(stream, token, '>');
      std::istringstream istream(token);
      while (std::getline(istream, token, ',')) {
        const auto begin = token.find(')');
        result += static_cast<char>(std::atoi(token.substr(begin + 1).c_str()));
      }
    };

    std::getline(stream, token, '<');
    std::getline(stream, ti.type, ',');

    parseString(ti.name);
    std::getline(stream, token, ',');

    parseString(ti.file);
    std::getline(stream, token, ',');

    std::getline(stream, token, ',');
    ti.line = std::atoi(token.c_str());

    parseString(ti.should);

    return ti;
  }
};

inline auto& tests() {
  static auto ts = symbols<TestCaseInfoParser>("_ZZN7testing2v16detail21SHOULD_REGISTER_GTEST");
  return ts;
}

template <class T>
class GTestAutoRegister {
  void MakeAndRegisterTestInfo(const TestCaseInfo& ti,
                               detail::type<TestInfo*(const char*, const char*, const char*, const char*, const void*,
                                                      void (*)(), void (*)(), internal::TestFactoryBase*)>) {
    if (ti.should.empty()) {
      internal::MakeAndRegisterTestInfo(ti.type.c_str(), ti.name.c_str(), nullptr, nullptr, internal::GetTestTypeId(),
                                        Test::SetUpTestCase, Test::TearDownTestCase, new detail::GTestFactoryImpl<T>{});
    } else {
      internal::MakeAndRegisterTestInfo((ti.type + ti.name).c_str(), (std::string{GUNIT_SHOULD_PREFIX} + ti.should).c_str(),
                                        nullptr, nullptr, internal::GetTestTypeId(), Test::SetUpTestCase,
                                        Test::TearDownTestCase, new detail::GTestFactoryImpl<T>{});
    }
  }

  template <class... Ts>
  void MakeAndRegisterTestInfo(const TestCaseInfo& ti, detail::type<TestInfo*(Ts...)>) {
    if (ti.should.empty()) {
      internal::MakeAndRegisterTestInfo(ti.type.c_str(), ti.name.c_str(), nullptr, nullptr, {ti.file.c_str(), ti.line},
                                        internal::GetTestTypeId(), Test::SetUpTestCase, Test::TearDownTestCase,
                                        new detail::GTestFactoryImpl<T>{});
    } else {
      internal::MakeAndRegisterTestInfo((ti.type + ti.name).c_str(), (std::string{GUNIT_SHOULD_PREFIX} + ti.should).c_str(),
                                        nullptr, nullptr, {ti.file.c_str(), ti.line}, internal::GetTestTypeId(),
                                        Test::SetUpTestCase, Test::TearDownTestCase, new detail::GTestFactoryImpl<T>{});
    }
  }

  bool RegisterShouldTestCase() {
    auto registered = false;
    for (const auto& ti : tests()) {
      if (get_type_name<typename T::TEST_TYPE>() == ti.type && T::TEST_NAME::c_str() == ti.name) {
        MakeAndRegisterTestInfo(ti, detail::type<decltype(::testing::internal::MakeAndRegisterTestInfo)>{});
        registered = true;
      }
    }
    return registered;
  }

 public:
  GTestAutoRegister() {
    if (!RegisterShouldTestCase()) {
      MakeAndRegisterTestInfo(
          {{}, get_type_name<typename T::TEST_TYPE>(), T::TEST_NAME::c_str(), T::TEST_FILE, T::TEST_LINE, {}},
          detail::type<decltype(::testing::internal::MakeAndRegisterTestInfo)>{});
    }
  }
};

template <class T, class = detail::is_complete<T>, class = detail::is_complete_base_of<Test, T>>
class GTest : public Test {
 protected:
  using SUT = std::unique_ptr<T>;

  explicit GTest(std::false_type) {}
  explicit GTest(std::true_type) { std::tie(sut, mocks) = make<SUT, StrictGMock>(); }

 public:
  GTest() : GTest(is_creatable<T>{}) {}

  template <class TMock>
  decltype(auto) mock() {
    return mocks.mock<TMock>();
  }

  mocks_t mocks;
  SUT sut;  // has to be after mocks
};

template <class T, class TAny>
class GTest<T, std::false_type, TAny> : public Test {
  template <class TMock>
  decltype(auto) mock() {
    return mocks.mock<TMock>();
  }

  mocks_t mocks;
};

template <class T>
class GTest<T, std::true_type, std::true_type> : public T {};

template <class T, class Name, class File, int Line, class Should>
bool SHOULD_REGISTER_GTEST() {
  static auto shouldRegister = true;
  return shouldRegister;
}
}  // detail

template <class T = detail::none_t>
class GTest : public detail::GTest<T> {};

}  // v1
}  // testing

#define GTEST(...) __GUNIT_CAT(GTEST_IMPL_, __GUNIT_SIZE(__VA_ARGS__))(__VA_ARGS__)

#define GTEST_IMPL_1(TYPE)                                                       \
  template <class...>                                                            \
  struct GTEST;                                                                  \
  template <>                                                                    \
  struct GTEST<TYPE> : ::testing::detail::GTest<TYPE> {                          \
    using TEST_TYPE = TYPE;                                                      \
    using TEST_NAME = ::testing::detail::string<>;                               \
    static constexpr auto TEST_FILE = __FILE__;                                  \
    static constexpr auto TEST_LINE = __LINE__;                                  \
    void test();                                                                 \
  };                                                                             \
  ::testing::detail::GTestAutoRegister<GTEST<TYPE>> __GUNIT_CAT(ar, __LINE__){}; \
  void GTEST<TYPE>::test()

#define GTEST_IMPL_2(TYPE, NAME)                                                                                         \
  auto __GUNIT_CAT(GTESET_IMPL_TEST_NAME, __LINE__)() { return __GUNIT_CAT(NAME, _gtest_string); }                       \
  template <class...>                                                                                                    \
  struct GTEST;                                                                                                          \
  template <>                                                                                                            \
  struct GTEST<TYPE, decltype(__GUNIT_CAT(GTESET_IMPL_TEST_NAME, __LINE__))> : ::testing::detail::GTest<TYPE> {          \
    using TEST_TYPE = TYPE;                                                                                              \
    using TEST_NAME = decltype(__GUNIT_CAT(GTESET_IMPL_TEST_NAME, __LINE__)());                                          \
    static constexpr auto TEST_FILE = __FILE__;                                                                          \
    static constexpr auto TEST_LINE = __LINE__;                                                                          \
    void test();                                                                                                         \
  };                                                                                                                     \
  ::testing::detail::GTestAutoRegister<GTEST<TYPE, decltype(__GUNIT_CAT(GTESET_IMPL_TEST_NAME, __LINE__))>> __GUNIT_CAT( \
      ar, __LINE__){};                                                                                                   \
  void GTEST<TYPE, decltype(__GUNIT_CAT(GTESET_IMPL_TEST_NAME, __LINE__))>::test()

#define SHOULD(NAME)                                                                                                           \
  if (::testing::detail::SHOULD_REGISTER_GTEST<TEST_TYPE, TEST_NAME, decltype(__GUNIT_CAT(__FILE__, _gtest_string)), __LINE__, \
                                               decltype(__GUNIT_CAT(NAME, _gtest_string))>() &&                                \
      std::string{GUNIT_SHOULD_PREFIX} + NAME == ::testing::UnitTest::GetInstance()->current_test_info()->name())
