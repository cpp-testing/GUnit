//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <gtest/gtest.h>
#include <cassert>
#include <cstring>
#include <fstream>
#include <functional>
#include <gherkin.hpp>
#include <json.hpp>
#include <regex>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>
#include "GUnit/Detail/Preprocessor.h"
#include "GUnit/Detail/Utility.h"

namespace testing {
inline namespace v1 {
struct StepIsNotImplemented : std::runtime_error {
  using std::runtime_error::runtime_error;
};
struct StepIsAmbiguous : std::runtime_error {
  using std::runtime_error::runtime_error;
};

namespace detail {
template <class T>
void MakeAndRegisterTestInfo(const T& test, const std::string& type, const std::string& name, const std::string& /*file*/,
                             int /*line*/,
                             detail::type<TestInfo*(const char*, const char*, const char*, const char*, const void*, void (*)(),
                                                    void (*)(), internal::TestFactoryBase*)>) {
  internal::MakeAndRegisterTestInfo(type.c_str(), name.c_str(), nullptr, nullptr, internal::GetTestTypeId(),
                                    Test::SetUpTestCase, Test::TearDownTestCase, test);
}

template <class T, class... Ts>
void MakeAndRegisterTestInfo(const T& test, const std::string& type, const std::string& name, const std::string& file, int line,
                             detail::type<TestInfo*(Ts...)>) {
  internal::MakeAndRegisterTestInfo(type.c_str(), name.c_str(), nullptr, nullptr, {file.c_str(), line},
                                    internal::GetTestTypeId(), Test::SetUpTestCase, Test::TearDownTestCase, test);
}

inline bool PatternMatchesString2(const char* pattern, const char* str) {
  switch (*pattern) {
    case '\0':
    case ':':  // Either ':' or '\0' marks the end of the pattern.
      return *str == '\0';
    case '?':  // Matches any single character.
      return *str != '\0' && PatternMatchesString2(pattern + 1, str + 1);
    case '*':  // Matches any string (possibly empty) of characters.
      return (*str != '\0' && PatternMatchesString2(pattern, str + 1)) || PatternMatchesString2(pattern + 1, str);
    default:  // Non-special character.  Matches itself.
      return *pattern == *str && PatternMatchesString2(pattern + 1, str + 1);
  }
}

struct context {
  static auto& step() {
    static auto i = 0;
    return i;
  }

  static auto& pickles() {
    static std::string p{};
    return p;
  }

  static auto& steps() {
    static std::unordered_map<std::string, std::function<void(const std::string&)>> s{};
    return s;
  }
};

inline auto parse(const std::string& feature, const std::wstring& content) {
  gherkin::parser parser{L"en"};
  gherkin::compiler compiler{feature};
  const auto gherkin_document = parser.parse(content);
  return compiler.compile(gherkin_document);
}

inline void run(const std::string& pickles) {
  const auto json = nlohmann::json::parse(pickles)["pickle"];
  for (const auto& expected_step : json["steps"]) {
    std::string line = expected_step["text"];
    auto found = false;
    for (const auto& given_step : context::steps()) {
      if (std::regex_match(line, std::regex{given_step.first})) {
        if (found) {
          throw StepIsAmbiguous{"STEP \"" + line + "\" is ambiguous!"};
        }
        std::cout << "\033[0;96m"
                  << "[ STEP     ] " << line << "\033[m" << '\n';
        given_step.second(line);
        found = true;
      }
    }

    if (not found) {
      throw StepIsNotImplemented{"STEP \"" + line + "\" not implemented!"};
    }
  }
}

template <class TSteps>
inline void parse_and_register(const std::string& name, const TSteps& steps, const std::string& feature) {
  const auto content = read_file(feature);
  gherkin::parser parser{L"en"};
  gherkin::compiler compiler{feature};
  const auto gherkin_document = parser.parse(content);
  const auto pickles = compiler.compile(gherkin_document);
  const auto ast = nlohmann::json::parse(compiler.ast(gherkin_document));
  for (const auto& pickle : pickles) {
    const std::string feature_name = ast["document"]["feature"]["name"];
    const std::string scenario_name = nlohmann::json::parse(pickle)["pickle"]["name"];

    if (PatternMatchesString2(name.c_str(), feature_name.c_str())) {
      class TestFactory : public internal::TestFactoryBase {
        class test : public Test {
         public:
          test(const TSteps& steps, const std::string& pickles) : steps{steps}, pickles{pickles} {}

          void TestBody() {
            context::pickles() = pickles;
            steps();
            std::cout << '\n';
            context::steps().clear();
          }

         private:
          TSteps steps;
          std::string pickles;
        };

       public:
        TestFactory(const TSteps& steps, const std::string& pickles) : steps{steps}, pickles{pickles} {}
        Test* CreateTest() override { return new test{steps, pickles}; }

       private:
        TSteps steps;
        std::string pickles;
      };

      MakeAndRegisterTestInfo(new TestFactory{steps, pickle}, feature_name, scenario_name, __FILE__, __LINE__,
                              detail::type<decltype(internal::MakeAndRegisterTestInfo)>{});
    }
  }
}

template <class TFeature>
struct steps {
  template <class TSteps>
  steps(const TSteps& s) {
    const auto scenario = std::getenv("SCENARIO");
    assert(scenario);
    for (const auto& feature : detail::split(scenario, ';')) {
      parse_and_register(TFeature::c_str(), s, feature);
    }
  }
};

template <class TRegex, class File, int Line>
class step {
 public:
  template <class TExpr>
  step(const TExpr& expr) {  // non explicit
    context::step()++;
    context::steps()[TRegex::c_str()] = [=](const std::string& st) {
      call(expr, st, TRegex::c_str(), detail::function_traits_t<TExpr>{});
    };
  }

  ~step() {
    if (not--context::step()) {
      run(context::pickles());
    }
  }

 private:
  template <class TExpr, class... Ts>
  void call(const TExpr& expr, const std::string& step, const std::string& regex, detail::type_list<Ts...> t) {
    std::regex pieces_regex{step};
    std::smatch pieces_match;
    (void)regex;
    assert(std::regex_match(step, pieces_match, std::regex{regex}));
    call_impl(expr, pieces_match, t, std::make_index_sequence<sizeof...(Ts)>{});
  }

  template <class TExpr, class TMatches, class... Ts, std::size_t... Ns>
  void call_impl(const TExpr& expr, const TMatches& matches, detail::type_list<Ts...>, std::index_sequence<Ns...>) {
    expr(detail::lexical_cast<Ts>(matches[Ns + 1].str().c_str())...);
  }
};

}  // detail
}  // v1
}  // testing

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#endif

#define STEPS(feature) __attribute__((unused))::testing::detail::steps<decltype(__GUNIT_CAT(feature, _gtest_string))>

#define __STEP_IMPL(txt)                                                                                      \
  __attribute__((unused))::testing::detail::step<decltype(__GUNIT_CAT(txt, _gtest_string)),                   \
                                                 decltype(__GUNIT_CAT(__FILE__ "", _gtest_string)), __LINE__> \
      __GUNIT_CAT(step_, __LINE__)

#define GIVEN __STEP_IMPL
#define WHEN __STEP_IMPL
#define THEN __STEP_IMPL

#define $Given __STEP_IMPL
#define $When __STEP_IMPL
#define $Then __STEP_IMPL
