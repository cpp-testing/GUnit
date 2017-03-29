//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <gtest/gtest.h>
#include <algorithm>
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

struct TestCaseInfo {
  bool disabled = false;
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
    const auto str = std::string{">()"};
    ti.symbol.replace(ti.symbol.find(str), str.length(), "|");
    std::string token;
    std::istringstream stream(ti.symbol);

    const auto parseString = [&](std::string& result, char end = '>') {
      std::getline(stream, token, end);
      if (token.find("::detail::string") != std::string::npos) {
        if (token.find(")") != std::string::npos) {
          std::istringstream istream(token);
          while (std::getline(istream, token, ',')) {
            const auto begin = token.find(')');
            result += static_cast<char>(std::atoi(token.substr(begin + 1).c_str()));
          }
        }
      } else {
        detail::trim(token);
        result = token;
      }
      std::getline(stream, token, ',');
    };

    std::getline(stream, token, '<');
    std::getline(stream, token, ',');
    ti.disabled = token == "true";
    parseString(ti.name);
    parseString(ti.file);
    std::getline(stream, token, ',');
    ti.line = std::atoi(token.c_str());
    parseString(ti.should);
    parseString(ti.type, '|');
    if (!ti.type.empty() && ti.type[0] == '"' && ti.type[ti.type.length() - 2] == '"') {
      ti.type = ti.type.substr(1, ti.type.length() - 3);
    }
    return ti;
  }
};

inline auto& tests() {
  static auto ts = symbols<TestCaseInfoParser>("_ZZN7testing2v16detail21SHOULD_REGISTER_GTEST");
  return ts;
}

template <bool DISABLED, class T>
class GTestAutoRegister {
  static auto IsDisabled(bool disabled) { return DISABLED || disabled ? "DISABLED_" : ""; }

  void MakeAndRegisterTestInfo(const TestCaseInfo& ti,
                               detail::type<TestInfo*(const char*, const char*, const char*, const char*, const void*,
                                                      void (*)(), void (*)(), internal::TestFactoryBase*)>) {
    if (ti.should.empty()) {
      internal::MakeAndRegisterTestInfo((IsDisabled(ti.disabled) + ti.type).c_str(), ti.name.c_str(), nullptr, nullptr,
                                        internal::GetTestTypeId(), Test::SetUpTestCase, Test::TearDownTestCase,
                                        new internal::TestFactoryImpl<T>{});
    } else {
      internal::MakeAndRegisterTestInfo((IsDisabled(ti.disabled) + ti.type + ti.name).c_str(),
                                        (std::string{GUNIT_SHOULD_PREFIX} + ti.should).c_str(), nullptr, nullptr,
                                        internal::GetTestTypeId(), Test::SetUpTestCase, Test::TearDownTestCase,
                                        new internal::TestFactoryImpl<T>{});
    }
  }

  template <class... Ts>
  void MakeAndRegisterTestInfo(const TestCaseInfo& ti, detail::type<TestInfo*(Ts...)>) {
    if (ti.should.empty()) {
      internal::MakeAndRegisterTestInfo((IsDisabled(ti.disabled) + ti.type).c_str(), ti.name.c_str(), nullptr, nullptr,
                                        {ti.file.c_str(), ti.line}, internal::GetTestTypeId(), Test::SetUpTestCase,
                                        Test::TearDownTestCase, new internal::TestFactoryImpl<T>{});
    } else {
      internal::MakeAndRegisterTestInfo((IsDisabled(ti.disabled) + ti.type + ti.name).c_str(),
                                        (std::string{GUNIT_SHOULD_PREFIX} + ti.should).c_str(), nullptr, nullptr,
                                        {ti.file.c_str(), ti.line}, internal::GetTestTypeId(), Test::SetUpTestCase,
                                        Test::TearDownTestCase, new internal::TestFactoryImpl<T>{});
    }
  }

  template <class TestType>
  static auto GetTypeName(detail::type<TestType>) {
    return get_type_name<TestType>();
  }

  template <char Quote, char... Chrs>
  static auto GetTypeName(detail::type<string<Quote, Chrs...>>) {
    static char str[] = {Chrs...};
    str[sizeof...(Chrs) - 2] = 0;  // last quote
    return str;
  }

  bool RegisterShouldTestCase() {
    auto registered = false;
    for (const auto& ti : tests()) {
      if (GetTypeName(detail::type<typename T::TEST_TYPE>{}) == ti.type && T::TEST_NAME::c_str() == ti.name) {
        MakeAndRegisterTestInfo(ti, detail::type<decltype(internal::MakeAndRegisterTestInfo)>{});
        registered = true;
      }
    }
    return registered;
  }

  bool RegisterShouldParamTestCase() {
    auto registered = false;
    for (const auto& ti : tests()) {
      if (GetTypeName(detail::type<typename T::TEST_TYPE>{}) == ti.type && T::TEST_NAME::c_str() == ti.name) {
        UnitTest::GetInstance()
            ->parameterized_test_registry()
            .GetTestCasePatternHolder<T>(ti.type.c_str(), {ti.file, ti.line})
            ->AddTestPattern((IsDisabled(ti.disabled) + ti.type).c_str(), std::string{GUNIT_SHOULD_PREFIX + ti.should}.c_str(),
                             new internal::TestMetaFactory<T>());
        registered = true;
      }
    }
    return registered;
  }

 public:
  GTestAutoRegister() {
    if (!RegisterShouldTestCase()) {
      MakeAndRegisterTestInfo({DISABLED,
                               {},
                               GetTypeName(detail::type<typename T::TEST_TYPE>{}),
                               T::TEST_NAME::c_str(),
                               T::TEST_FILE,
                               T::TEST_LINE,
                               {}},
                              detail::type<decltype(internal::MakeAndRegisterTestInfo)>{});
    }
  }

  template <class TEval, class TGenerateNames>
  explicit GTestAutoRegister(const TEval& eval, const TGenerateNames& genNames) {
    if (!RegisterShouldParamTestCase()) {
      UnitTest::GetInstance()
          ->parameterized_test_registry()
          .GetTestCasePatternHolder<T>(GetTypeName(detail::type<typename T::TEST_TYPE>{}), {T::TEST_FILE, T::TEST_LINE})
          ->AddTestPattern(GetTypeName(detail::type<typename T::TEST_TYPE>{}),
                           GetTypeName(detail::type<typename T::TEST_TYPE>{}), new internal::TestMetaFactory<T>());
    }

    UnitTest::GetInstance()
        ->parameterized_test_registry()
        .GetTestCasePatternHolder<T>(GetTypeName(detail::type<typename T::TEST_TYPE>{}), {T::TEST_FILE, T::TEST_LINE})
        ->AddTestCaseInstantiation((std::string{IsDisabled(DISABLED)} + T::TEST_NAME::c_str()).c_str(), eval, genNames,
                                   T::TEST_FILE, T::TEST_LINE);
  }
};

template <class T, class TParamType, class = detail::is_complete<T>, class = detail::is_complete_base_of<Test, T>>
class GTest : public std::conditional_t<std::is_same<TParamType, void>::value, Test, TestWithParam<TParamType>> {
  explicit GTest(std::false_type) {}
  explicit GTest(std::true_type) { std::tie(sut, mocks) = make<SUT, StrictGMock>(); }

 public:
  using SUT = std::unique_ptr<T>;

  GTest() : GTest(is_creatable<T>{}) {}

  template <class TMock>
  decltype(auto) mock() {
    return mocks.mock<TMock>();
  }

  mocks_t mocks;
  SUT sut;  // has to be after mocks
};

template <class T, class TParamType, class TAny>
class GTest<T, TParamType, std::false_type, TAny> : public Test {
 public:
  template <class TMock>
  decltype(auto) mock() {
    return mocks.mock<TMock>();
  }

  mocks_t mocks;
};

template <class T, class TParamType>
class GTest<T, TParamType, std::true_type, std::true_type> : public T {};

template <bool Disabled, class Name, class File, int Line, class Should, class T>
bool SHOULD_REGISTER_GTEST() {
  static auto shouldRegister = true;
  return shouldRegister;
}
}  // detail

template <class T = detail::none_t, class TParamType = void>
class GTest : public detail::GTest<T, TParamType> {};

}  // v1
}  // testing

#define __GTEST_IMPL(DISABLED, TYPE, NAME, PARAMS, ...)                                                                   \
  struct __GUNIT_CAT(GTEST_STRING_, __LINE__) {                                                                           \
    const char* chrs = #TYPE;                                                                                             \
  };                                                                                                                      \
  using __GUNIT_CAT(GTEST_TYPE_, __LINE__) =                                                                              \
      std::conditional_t<#TYPE[(0)] == '"', decltype(::testing::detail::make_string<__GUNIT_CAT(GTEST_STRING_, __LINE__), \
                                                                                    sizeof(#TYPE)>::type()),              \
                         __typeof__(TYPE)>;                                                                               \
  template <class...>                                                                                                     \
  struct GTEST;                                                                                                           \
  template <>                                                                                                             \
  struct GTEST<__GUNIT_CAT(GTEST_TYPE_, __LINE__), NAME>                                                                  \
      : ::testing::detail::GTest<__GUNIT_CAT(GTEST_TYPE_, __LINE__),                                                      \
                                 ::testing::detail::apply_t<std::common_type_t, decltype(PARAMS)>> {                      \
    using TEST_TYPE = __GUNIT_CAT(GTEST_TYPE_, __LINE__);                                                                 \
    using TEST_NAME = NAME;                                                                                               \
                                                                                                                          \
    static constexpr auto TEST_FILE = __FILE__;                                                                           \
    static constexpr auto TEST_LINE = __LINE__;                                                                           \
    void TestBody();                                                                                                      \
  };                                                                                                                      \
  static ::testing::detail::GTestAutoRegister<DISABLED, GTEST<__GUNIT_CAT(GTEST_TYPE_, __LINE__), NAME>> __GUNIT_CAT(     \
      ar, __LINE__){__VA_ARGS__};                                                                                         \
  void GTEST<__GUNIT_CAT(GTEST_TYPE_, __LINE__), NAME>::TestBody()

#define __GTEST_IMPL_1(DISABLED, TYPE) \
  __GTEST_IMPL(DISABLED, TYPE, ::testing::detail::string<>, ::testing::detail::type<void>{}, )
#define __GTEST_IMPL_2(DISABLED, TYPE, NAME)                                                 \
  using __GUNIT_CAT(GTEST_TEST_NAME, __LINE__) = decltype(__GUNIT_CAT(NAME, _gtest_string)); \
  __GTEST_IMPL(DISABLED, TYPE, __GUNIT_CAT(GTEST_TEST_NAME, __LINE__), ::testing::detail::type<void>{}, )

#define __GTEST_IMPL_3(DISABLED, TYPE, NAME, PARAMS)                                                                        \
  using __GUNIT_CAT(GTEST_TEST_NAME, __LINE__) = decltype(__GUNIT_CAT(NAME, _gtest_string));                                \
  static ::testing::internal::ParamGenerator<::testing::detail::apply_t<std::common_type_t, decltype(PARAMS)>> __GUNIT_CAT( \
      GTEST_EVAL, __LINE__)() {                                                                                             \
    return PARAMS;                                                                                                          \
  }                                                                                                                         \
  static std::string __GUNIT_CAT(GTEST_GENERATE_NAMES, __LINE__)(                                                           \
      const ::testing::TestParamInfo<::testing::detail::apply_t<std::common_type_t, decltype(PARAMS)>>& info) {             \
    return ::testing::internal::GetParamNameGen<::testing::detail::apply_t<std::common_type_t, decltype(PARAMS)>>()(info);  \
  }                                                                                                                         \
  __GTEST_IMPL(DISABLED, TYPE, __GUNIT_CAT(GTEST_TEST_NAME, __LINE__), PARAMS, &__GUNIT_CAT(GTEST_EVAL, __LINE__),          \
               &__GUNIT_CAT(GTEST_GENERATE_NAMES, __LINE__))

#define GTEST(...) __GUNIT_CAT(__GTEST_IMPL_, __GUNIT_SIZE(__VA_ARGS__))(false, __VA_ARGS__)
#define DISABLED_GTEST(...) __GUNIT_CAT(__GTEST_IMPL_, __GUNIT_SIZE(__VA_ARGS__))(true, __VA_ARGS__)

#define SHOULD(NAME)                                                                                                       \
  if (::testing::detail::SHOULD_REGISTER_GTEST<false, TEST_NAME, decltype(__GUNIT_CAT(__FILE__, _gtest_string)), __LINE__, \
                                               decltype(__GUNIT_CAT(NAME, _gtest_string)), TEST_TYPE>() &&                 \
      std::string{::testing::UnitTest::GetInstance()->current_test_info()->name()}.find(std::string{GUNIT_SHOULD_PREFIX} + \
                                                                                        NAME) != std::string::npos)

#define DISABLED_SHOULD(NAME)                                                                                              \
  if (::testing::detail::SHOULD_REGISTER_GTEST<true, TEST_NAME, decltype(__GUNIT_CAT(__FILE__, _gtest_string)), __LINE__,  \
                                               decltype(__GUNIT_CAT(NAME, _gtest_string)), TEST_TYPE>() &&                 \
      std::string{::testing::UnitTest::GetInstance()->current_test_info()->name()}.find(std::string{GUNIT_SHOULD_PREFIX} + \
                                                                                        NAME) != std::string::npos)
