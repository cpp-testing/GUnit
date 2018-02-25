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
#include "GUnit/Detail/RegexUtils.h"
#include "GUnit/Detail/StringUtils.h"
#include "GUnit/Detail/TermUtils.h"
#include "GUnit/Detail/TypeTraits.h"
#include "GUnit/GMake.h"
#include "GUnit/GMock.h"

namespace testing {
inline namespace v1 {
namespace detail {

struct TestRun {
  std::string should_param = GetShouldParam();
  bool next = true;

  std::string GetShouldParam() const {
    const auto sep = GTEST_FLAG(filter).find(":");
    return sep == std::string::npos ? "*" : GTEST_FLAG(filter).substr(sep + 1);
  }

  bool run(const std::string& type, const std::string& name, int line,
           bool disabled = false) {
    if (next) {
      return false;
    }

    const auto indented_name = "    " + name;

    const auto result =
        line > test_line && FilterMatchesShould(name, should_param);
    if (result) {
      static const bool is_stdout_tty = ShouldUseColor(
          internal::posix::IsATTY(internal::posix::FileNo(stdout)) != 0);
      const auto colorize = ShouldUseColor(is_stdout_tty);

      if (disabled && !GTEST_FLAG(also_run_disabled_tests)) {
        if (colorize) {
          std::cout << "\33[0;33m\033[2m";
        }
        std::cout << "[ DISABLED ] ";
        if (colorize) {
          std::cout << "\033[m";  // Resets the terminal to default.
        }
        std::cout << indented_name << std::endl;
        return false;
      }

      if (colorize) {
        std::cout << "\033[0;33m";
      }
      std::cout << "[ " << std::left << std::setw(8) << type << " ] ";
      if (colorize) {
        std::cout << "\033[m";  // Resets the terminal to default.
      }
      std::cout << indented_name << std::endl;
      test_line = line;
      next = true;
    }
    return result;
  }

  int test_line = 0;
};

template <bool DISABLED, class T>
class GTestAutoRegister {
  static auto IsDisabled(bool disabled) {
    return DISABLED || disabled ? "DISABLED_" : "";
  }

  void MakeAndRegisterTestInfo(
      bool disabled, const std::string& type, const std::string& name,
      const std::string& /*file*/, int /*line*/,
      detail::type<TestInfo*(const char*, const char*, const char*, const char*,
                             const void*, void (*)(), void (*)(),
                             internal::TestFactoryBase*)>) {
    internal::MakeAndRegisterTestInfo(
        (IsDisabled(disabled) + type).c_str(), name.c_str(), nullptr, nullptr,
        internal::GetTestTypeId(), Test::SetUpTestCase, Test::TearDownTestCase,
        new internal::TestFactoryImpl<T>{});
  }

  template <class... Ts>
  void MakeAndRegisterTestInfo(bool disabled, const std::string& type,
                               const std::string& name, const std::string& file,
                               int line, detail::type<TestInfo*(Ts...)>) {
    internal::MakeAndRegisterTestInfo(
        (IsDisabled(disabled) + type).c_str(), name.c_str(), nullptr, nullptr,
        {file.c_str(), line}, internal::GetTestTypeId(), Test::SetUpTestCase,
        Test::TearDownTestCase, new internal::TestFactoryImpl<T>{});
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

 public:
  GTestAutoRegister() {
    MakeAndRegisterTestInfo(
        DISABLED, GetTypeName(detail::type<typename T::TEST_TYPE>{}),
        T::TEST_NAME::c_str(), T::TEST_FILE, T::TEST_LINE,
        detail::type<decltype(internal::MakeAndRegisterTestInfo)>{});
  }

  template <class TEval, class TGenerateNames>
  GTestAutoRegister(const TEval& eval, const TGenerateNames& genNames) {
    UnitTest::GetInstance()
        ->parameterized_test_registry()
        .GetTestCasePatternHolder<T>(
            GetTypeName(detail::type<typename T::TEST_TYPE>{}),
            {T::TEST_FILE, T::TEST_LINE})
        ->AddTestPattern(GetTypeName(detail::type<typename T::TEST_TYPE>{}),
                         GetTypeName(detail::type<typename T::TEST_TYPE>{}),
                         new internal::TestMetaFactory<T>());

    UnitTest::GetInstance()
        ->parameterized_test_registry()
        .GetTestCasePatternHolder<T>(
            GetTypeName(detail::type<typename T::TEST_TYPE>{}),
            {T::TEST_FILE, T::TEST_LINE})
        ->AddTestCaseInstantiation(
            (std::string{IsDisabled(DISABLED)} + T::TEST_NAME::c_str()).c_str(),
            eval, genNames, T::TEST_FILE, T::TEST_LINE);
  }
};

template <class T, class TParamType, class = detail::is_complete<T>,
          class = detail::is_complete_base_of<Test, T>>
class GTest : public std::conditional_t<std::is_same<TParamType, void>::value,
                                        Test, TestWithParam<TParamType>> {
  explicit GTest(std::false_type) {}
  explicit GTest(std::true_type) {
    std::tie(sut, mocks) = make<SUT, StrictGMock>();
  }

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

}  // detail

template <class T = detail::none_t, class TParamType = void>
class GTest : public detail::GTest<T, TParamType> {};

}  // v1
}  // testing

#define __GTEST_IMPL(DISABLED, TYPE, NAME, PARAMS, ...)                       \
  struct __GUNIT_CAT(GTEST_STRING_, __LINE__) {                               \
    const char* chrs = #TYPE;                                                 \
  };                                                                          \
  using __GUNIT_CAT(GTEST_TYPE_, __LINE__) = std::conditional_t<              \
      #TYPE[(0)] == '"',                                                      \
      decltype(::testing::detail::make_string<                                \
               __GUNIT_CAT(GTEST_STRING_, __LINE__), sizeof(#TYPE)>::type()), \
      __typeof__(TYPE)>;                                                      \
  template <class...>                                                         \
  struct GTEST;                                                               \
  template <>                                                                 \
  struct GTEST<__GUNIT_CAT(GTEST_TYPE_, __LINE__), NAME>                      \
      : ::testing::detail::GTest<__GUNIT_CAT(GTEST_TYPE_, __LINE__),          \
                                 ::testing::detail::apply_t<                  \
                                     std::common_type_t, decltype(PARAMS)>> { \
    using TEST_TYPE = __GUNIT_CAT(GTEST_TYPE_, __LINE__);                     \
    using TEST_NAME = NAME;                                                   \
    static constexpr auto TEST_FILE = __FILE__;                               \
    static constexpr auto TEST_LINE = __LINE__;                               \
    void TestBodyImpl(::testing::detail::TestRun&);                           \
    void TestBody() {                                                         \
      ::testing::detail::TestRun tr;                                          \
      while (tr.next) {                                                       \
        tr.next = false;                                                      \
        GTEST test;                                                           \
        test.SetUp();                                                         \
        test.TestBodyImpl(tr);                                                \
        test.TearDown();                                                      \
      };                                                                      \
    }                                                                         \
  };                                                                          \
  static ::testing::detail::GTestAutoRegister<                                \
      DISABLED, GTEST<__GUNIT_CAT(GTEST_TYPE_, __LINE__), NAME>>              \
      __GUNIT_CAT(ar, __LINE__){__VA_ARGS__};                                 \
  void GTEST<__GUNIT_CAT(GTEST_TYPE_, __LINE__), NAME>::TestBodyImpl(         \
      ::testing::detail::TestRun& tr_gtest __attribute__((unused)))

#define __GTEST_IMPL_1(DISABLED, TYPE)                      \
  __GTEST_IMPL(DISABLED, TYPE, ::testing::detail::string<>, \
               ::testing::detail::type<void>{}, )
#define __GTEST_IMPL_2(DISABLED, TYPE, NAME)                           \
  using __GUNIT_CAT(GTEST_TEST_NAME, __LINE__) =                       \
      decltype(__GUNIT_CAT(NAME, _gtest_string));                      \
  __GTEST_IMPL(DISABLED, TYPE, __GUNIT_CAT(GTEST_TEST_NAME, __LINE__), \
               ::testing::detail::type<void>{}, )

#define __GTEST_IMPL_3(DISABLED, TYPE, NAME, PARAMS)                           \
  using __GUNIT_CAT(GTEST_TEST_NAME, __LINE__) =                               \
      decltype(__GUNIT_CAT(NAME, _gtest_string));                              \
  static ::testing::internal::ParamGenerator<                                  \
      ::testing::detail::apply_t<std::common_type_t, decltype(PARAMS)>>        \
  __GUNIT_CAT(GTEST_EVAL, __LINE__)() {                                        \
    return PARAMS;                                                             \
  }                                                                            \
  static std::string __GUNIT_CAT(GTEST_GENERATE_NAMES, __LINE__)(              \
      const ::testing::TestParamInfo<                                          \
          ::testing::detail::apply_t<std::common_type_t, decltype(PARAMS)>>&   \
          info) {                                                              \
    return ::testing::internal::GetParamNameGen<                               \
        ::testing::detail::apply_t<std::common_type_t, decltype(PARAMS)>>()(   \
        info);                                                                 \
  }                                                                            \
  __GTEST_IMPL(DISABLED, TYPE, __GUNIT_CAT(GTEST_TEST_NAME, __LINE__), PARAMS, \
               &__GUNIT_CAT(GTEST_EVAL, __LINE__),                             \
               &__GUNIT_CAT(GTEST_GENERATE_NAMES, __LINE__))

#define GTEST(...) \
  __GUNIT_CAT(__GTEST_IMPL_, __GUNIT_SIZE(__VA_ARGS__))(false, __VA_ARGS__)
#define DISABLED_GTEST(...) \
  __GUNIT_CAT(__GTEST_IMPL_, __GUNIT_SIZE(__VA_ARGS__))(true, __VA_ARGS__)

#define SHOULD(NAME) if (tr_gtest.run("SHOULD", NAME, __LINE__))
#define DISABLED_SHOULD(NAME) if (tr_gtest.run("SHOULD", NAME, __LINE__, true))
