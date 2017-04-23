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

namespace testing {
inline namespace v1 {
namespace detail {

inline bool PatternMatchesString(const char* pattern, const char* str) {
  switch (*pattern) {
    case '\0':
    case ':':  // Either ':' or '\0' marks the end of the pattern.
      return *str == '\0';
    case '?':  // Matches any single character.
      return *str != '\0' && PatternMatchesString(pattern + 1, str + 1);
    case '*':  // Matches any string (possibly empty) of characters.
      return (*str != '\0' && PatternMatchesString(pattern, str + 1)) || PatternMatchesString(pattern + 1, str);
    default:  // Non-special character.  Matches itself.
      return *pattern == *str && PatternMatchesString(pattern + 1, str + 1);
  }
}

inline bool MatchesFilter(const std::string& name, const char* filter) {
  const char* cur_pattern = filter;
  for (;;) {
    if (PatternMatchesString(cur_pattern, name.c_str())) {
      return true;
    }

    // Finds the next pattern in the filter.
    cur_pattern = strchr(cur_pattern, ':');

    // Returns if no more pattern can be found.
    if (cur_pattern == nullptr) {
      return false;
    }

    // Skips the pattern separater (the ':' character).
    cur_pattern++;
  }
}

inline bool FilterMatchesShould(const std::string& name, const std::string& should) {
  // Split --gtest_filter at '-', if there is one, to separate into
  // positive filter and negative filter portions
  const char* const p = should.c_str();
  const char* const dash = strchr(p, '-');
  std::string positive;
  std::string negative;
  if (dash == nullptr) {
    positive = should.c_str();  // Whole string is a positive filter
    negative = "";
  } else {
    positive = std::string(p, dash);   // Everything up to the dash
    negative = std::string(dash + 1);  // Everything after the dash
    if (positive.empty()) {
      // Treat '-test1' as the same as '*-test1'
      positive = "*";
    }
  }

  return MatchesFilter(name, positive.c_str()) && !MatchesFilter(name, negative.c_str());
}

// This function is lifted from GTest's internals.
// Returns true iff colors should be used in the output.
bool ShouldUseColor(bool stdout_is_tty) {
  using internal::String;

  const char* const gtest_color = GTEST_FLAG(color).c_str();

  if (String::CaseInsensitiveCStringEquals(gtest_color, "auto")) {
    const char* const term = internal::posix::GetEnv("TERM");
    const bool term_supports_color = String::CStringEquals(term, "xterm") || String::CStringEquals(term, "xterm-color") ||
                                     String::CStringEquals(term, "xterm-256color") || String::CStringEquals(term, "screen") ||
                                     String::CStringEquals(term, "screen-256color") || String::CStringEquals(term, "tmux") ||
                                     String::CStringEquals(term, "tmux-256color") ||
                                     String::CStringEquals(term, "rxvt-unicode") ||
                                     String::CStringEquals(term, "rxvt-unicode-256color") ||
                                     String::CStringEquals(term, "linux") || String::CStringEquals(term, "cygwin");
    return stdout_is_tty && term_supports_color;
  }

  return String::CaseInsensitiveCStringEquals(gtest_color, "yes") ||
         String::CaseInsensitiveCStringEquals(gtest_color, "true") || String::CaseInsensitiveCStringEquals(gtest_color, "t") ||
         String::CStringEquals(gtest_color, "1");
  // We take "yes", "true", "t", and "1" as meaning "yes".  If the value is neither one of these nor "auto", we treat it as "no"
  // to be conservative.
}

struct TestRun {
  std::string should = GetShouldParam();
  bool once = true;

  std::string GetShouldParam() const {
    const auto sep = GTEST_FLAG(filter).find(":");
    return sep == std::string::npos
      ? "*"
      : GTEST_FLAG(filter).substr(sep + 1);
  }

  bool run(bool disabled, const std::string& name, int line) {
    if (once) {
      return false;
    }

    static const bool is_stdout_tty = ShouldUseColor(internal::posix::IsATTY(internal::posix::FileNo(stdout)) != 0);

    auto colorize = ShouldUseColor(is_stdout_tty);

    if (disabled && !GTEST_FLAG(also_run_disabled_tests)) {
      if (colorize) {
        std::cout << "\033[0;33m";
      }
      std::cout << "[ DISABLED ] " << name << std::endl;
      if (colorize) {
        std::cout << "\033[m";  // Resets the terminal to default.
      }
      return false;
    }

    const auto result = line > test_line && FilterMatchesShould(name, should);
    if (result) {
      if (colorize) {
        std::cout << "\033[0;33m";
      }
      std::cout << "[ SHOULD   ] " << name << std::endl;
      if (colorize) {
        std::cout << "\033[m";  // Resets the terminal to default.
      }
      test_line = line;
      once = true;
    }
    return result;
  }

  int test_line = 0;
};

template <bool DISABLED, class T>
class GTestAutoRegister {
  static auto IsDisabled(bool disabled) { return DISABLED || disabled ? "DISABLED_" : ""; }

  void MakeAndRegisterTestInfo(bool disabled, const std::string& type, const std::string& name, const std::string& /*file*/,
                               int /*line*/,
                               detail::type<TestInfo*(const char*, const char*, const char*, const char*, const void*,
                                                      void (*)(), void (*)(), internal::TestFactoryBase*)>) {
    internal::MakeAndRegisterTestInfo((IsDisabled(disabled) + type).c_str(), name.c_str(), nullptr, nullptr,
                                      internal::GetTestTypeId(), Test::SetUpTestCase, Test::TearDownTestCase,
                                      new internal::TestFactoryImpl<T>{});
  }

  template <class... Ts>
  void MakeAndRegisterTestInfo(bool disabled, const std::string& type, const std::string& name, const std::string& file,
                               int line, detail::type<TestInfo*(Ts...)>) {
    internal::MakeAndRegisterTestInfo((IsDisabled(disabled) + type).c_str(), name.c_str(), nullptr, nullptr,
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
    MakeAndRegisterTestInfo(DISABLED, GetTypeName(detail::type<typename T::TEST_TYPE>{}), T::TEST_NAME::c_str(), T::TEST_FILE,
                            T::TEST_LINE, detail::type<decltype(internal::MakeAndRegisterTestInfo)>{});
  }

  template <class TEval, class TGenerateNames>
  GTestAutoRegister(const TEval& eval, const TGenerateNames& genNames) {
    UnitTest::GetInstance()
        ->parameterized_test_registry()
        .GetTestCasePatternHolder<T>(GetTypeName(detail::type<typename T::TEST_TYPE>{}), {T::TEST_FILE, T::TEST_LINE})
        ->AddTestPattern(GetTypeName(detail::type<typename T::TEST_TYPE>{}), GetTypeName(detail::type<typename T::TEST_TYPE>{}),
                         new internal::TestMetaFactory<T>());

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
    static constexpr auto TEST_FILE = __FILE__;                                                                           \
    static constexpr auto TEST_LINE = __LINE__;                                                                           \
    void TestBodyImpl(::testing::detail::TestRun&);                                                                       \
    void TestBody() {                                                                                                     \
      ::testing::detail::TestRun tr;                                                                                      \
      while (tr.once) {                                                                                                   \
        tr.once = false;                                                                                                  \
        GTEST test;                                                                                                       \
        test.SetUp();                                                                                                     \
        test.TestBodyImpl(tr);                                                                                            \
        test.TearDown();                                                                                                  \
      };                                                                                                                  \
    }                                                                                                                     \
  };                                                                                                                      \
  static ::testing::detail::GTestAutoRegister<DISABLED, GTEST<__GUNIT_CAT(GTEST_TYPE_, __LINE__), NAME>> __GUNIT_CAT(     \
      ar, __LINE__){__VA_ARGS__};                                                                                         \
  void GTEST<__GUNIT_CAT(GTEST_TYPE_, __LINE__), NAME>::TestBodyImpl(::testing::detail::TestRun& tr_gtest                 \
                                                                     __attribute__((unused)))

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

#define SHOULD(NAME) if (tr_gtest.run(false, NAME, __LINE__))
#define DISABLED_SHOULD(NAME) if (tr_gtest.run(true, NAME, __LINE__))
