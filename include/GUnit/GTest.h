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

namespace testing {
inline namespace v1 {
namespace detail {

template <class T>
class GTestFactoryImpl final : public internal::TestFactoryBase {
  class TestImpl final : public T, public Test {
   public:
    void TestBody() override { T::test(); }
  };

 public:
  Test* CreateTest() override { return new TestImpl{}; }
};

inline auto& getSymbols() {
  static auto output = symbols("void testing::v1::detail::SHOULD_REGISTER_GTEST");
  return output;
}

template <class T>
class GTestAutoRegister {
  struct TestInfo {
    std::string type;
    std::string name;
    std::string file;
    unsigned long long line;
    std::string should;
  };

  auto parse(const std::string& line) {
    TestInfo ti;
    std::string token;

    const auto parseString = [&](std::string& result) {
      std::istringstream stream(token);
      while (std::getline(stream, token, ',')) {
        const auto begin = token.find(')');
        result += static_cast<char>(std::atoi(token.substr(begin + 1).c_str()));
      }
    };

    std::istringstream stream(line);
    std::getline(stream, ti.type, ',');

    std::getline(stream, token, '<');
    std::getline(stream, token, '>');

    parseString(ti.name);
    std::getline(stream, token, ',');

    parseString(ti.file);
    std::getline(stream, token, ',');

    std::getline(stream, token, ',');
    ti.line = std::atoi(token.c_str());

    std::getline(stream, token, '<');
    std::getline(stream, token, '>');

    parseString(ti.should);

    return ti;
  }

 public:
  GTestAutoRegister() {
    for (const auto& testInfo : getSymbols()) {
      const auto test = parse(testInfo);
      if (get_type_name<typename T::TEST_TYPE>() == test.type && T::TEST_NAME::c_str() == test.name) {
        ::testing::internal::MakeAndRegisterTestInfo(
            (test.type + test.name).c_str(), (std::string{"should "} + test.should).c_str(), nullptr, nullptr,
            ::testing::internal::CodeLocation(test.file.c_str(), test.line), ::testing::internal::GetTestTypeId(),
            ::testing::Test::SetUpTestCase, ::testing::Test::TearDownTestCase, new ::testing::detail::GTestFactoryImpl<T>{});
      }
    }
  }
};
template <class T, class = detail::is_complete<T>>
class GTest {
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

template <class T>
class GTest<T, std::false_type> {};

template <class T, class Name, class File, unsigned long long Line, class Should>
void SHOULD_REGISTER_GTEST() {
  static auto once = true;
  once = false;
  (void)once;
}
}  // detail

template <class T = detail::none_t>
class GTest : public detail::GTest<T>, public Test {};
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
    void test();                                                                 \
  };                                                                             \
  ::testing::detail::GTestAutoRegister<GTEST<TYPE>> __GUNIT_CAT(ar, __LINE__){}; \
  void GTEST<TYPE>::test()

#define GTEST_IMPL_2(TYPE, NAME)                                                                                         \
  auto __GUNIT_CAT(GTESET_IMPL_TEST_NAME, __LINE__)() {                                                                  \
    using namespace ::testing::detail;                                                                                   \
    return __GUNIT_CAT(NAME, _s);                                                                                        \
  }                                                                                                                      \
  template <class...>                                                                                                    \
  struct GTEST;                                                                                                          \
  template <>                                                                                                            \
  struct GTEST<TYPE, decltype(__GUNIT_CAT(GTESET_IMPL_TEST_NAME, __LINE__))> : ::testing::detail::GTest<TYPE> {          \
    using TEST_TYPE = TYPE;                                                                                              \
    using TEST_NAME = decltype(__GUNIT_CAT(GTESET_IMPL_TEST_NAME, __LINE__)());                                          \
    void test();                                                                                                         \
  };                                                                                                                     \
  ::testing::detail::GTestAutoRegister<GTEST<TYPE, decltype(__GUNIT_CAT(GTESET_IMPL_TEST_NAME, __LINE__))>> __GUNIT_CAT( \
      ar, __LINE__){};                                                                                                   \
  void GTEST<TYPE, decltype(__GUNIT_CAT(GTESET_IMPL_TEST_NAME, __LINE__))>::test()

#define SHOULD(NAME)                                                                         \
  using namespace ::testing::detail;                                                         \
  SHOULD_REGISTER_GTEST<TEST_TYPE, TEST_NAME, decltype(__GUNIT_CAT(__FILE__, _s)), __LINE__, \
                        decltype(__GUNIT_CAT(NAME, _s))>();                                  \
  if (std::string{"should "} + NAME == ::testing::UnitTest::GetInstance()->current_test_info()->name())
