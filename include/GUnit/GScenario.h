//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <dirent.h>
#include <cassert>
#include <cstring>
#include <fstream>
#include <functional>
#include <gherkin.hpp>
#include <json.hpp>
#include <regex>
#include <unordered_map>
#include <utility>
#include "GUnit/Detail/Preprocessor.h"
#include "GUnit/Detail/Utility.h"

namespace testing {
inline namespace v1 {
struct steps {
  static auto& get() {
    using steps_t = std::unordered_map<std::string, std::function<void(const std::string&)>>;
    static std::unordered_map<void*, steps_t> s{};
    return s;
  }
};

struct self {
  explicit self(void* ptr) { *self_ptr() = ptr; }
  static void** self_ptr() {
    static void* ptr{};
    return &ptr;
  }
};

template <class TRegex, class File, int Line>
class step {
 public:
  template <class TExpr>
  step(const TExpr& expr) {  // non explicit
    steps::get()[*self::self_ptr()][TRegex::c_str()] = [=](const std::string& st) {
      call(expr, st, TRegex::c_str(), detail::function_traits_t<TExpr>{});
    };
  }

 private:
  template <class TExpr, class... Ts>
  void call(const TExpr& expr, const std::string& step, const std::string& regex, detail::type_list<Ts...> t) {
    std::regex pieces_regex{step};
    std::smatch pieces_match;
    assert(std::regex_match(step, pieces_match, std::regex{regex}));
    using ft_t = detail::function_traits<TExpr>;
    call_impl(expr, pieces_match, detail::type<typename ft_t::base_type>{}, typename ft_t::is_lambda_expr{}, t,
              std::make_index_sequence<sizeof...(Ts)>{});
  }

  template <class TExpr, class TMatches, class T, class... Ts, std::size_t... Ns>
  void call_impl(const TExpr& expr, const TMatches& matches, detail::type<T>, std::false_type, detail::type_list<Ts...>,
                 std::index_sequence<Ns...>) {
    ((static_cast<T*>(*self::self_ptr()))->*expr)(detail::lexical_cast<Ts>(matches[Ns + 1].str().c_str())...);
  }

  template <class TExpr, class TMatches, class T, class... Ts, std::size_t... Ns>
  void call_impl(const TExpr& expr, const TMatches& matches, detail::type<T>, std::true_type, detail::type_list<Ts...>,
                 std::index_sequence<Ns...>) {
    expr(detail::lexical_cast<Ts>(matches[Ns + 1].str().c_str())...);
  }
};

namespace detail {
inline auto parse(const std::string& feature, const std::wstring& content) {
  gherkin::parser parser{L"en"};
  gherkin::compiler compiler{feature};
  const auto gherkin_document = parser.parse(content);
  return compiler.compile(gherkin_document);
}

template <class TSteps, class TJson>
inline void run_pickle(const std::string& feature, const TJson& json) {
  TSteps steps{};
  for (const auto& expected_step : json["steps"]) {
    std::string line = expected_step["text"];
    for (const auto& given_step : steps::get()[&steps]) {
      if (std::regex_match(line, std::regex{given_step.first})) {
        std::cout << '\t' << line << "\t" << feature << ":" << expected_step["locations"] << '\n';
        given_step.second(line);
      }
    }
  }
  steps::get()[&steps].clear();
  std::cout << '\n';
}

template <class TSteps>
inline void run(const std::string& feature, const std::vector<std::string>& pickles) {
  for (const auto& pickle : pickles) {
    run_pickle<TSteps>(feature, nlohmann::json::parse(pickle)["pickle"]);
  }
}

template <class TSteps>
inline void parse_and_run(const std::string& feature) {
  const auto content = read_file(feature);
  const auto pickles = parse(feature, content);
  run<TSteps>(feature, pickles);
}

template <class TSteps>
inline void run_scenarios(const std::string& feature) {
  if (is_dir(feature.c_str())) {
    auto dp = opendir(feature.c_str());
    assert(dp);
    dirent* entry{};
    while ((entry = readdir(dp))) {
      if (is_file(feature + "/" + entry->d_name)) {
        parse_and_run<TSteps>(feature + "/" + entry->d_name);
      }
    }
    closedir(dp);
  } else {
    parse_and_run<TSteps>(feature);
  }
}

template <class TSteps>
inline auto get_feature_scenario(const std::string& feature) {
  std::vector<std::string> scenarios{};
  const auto content = read_file(feature);
  gherkin::parser parser{L"en"};
  gherkin::compiler compiler{feature};
  const auto gherkin_document = parser.parse(content);
  const auto pickles = compiler.compile(gherkin_document);
  const auto ast = nlohmann::json::parse(compiler.ast(gherkin_document));
  for (const auto& pickle : pickles) {
    const std::string feature_name = ast["document"]["feature"]["name"];
    const std::string scenario_name = nlohmann::json::parse(pickle)["pickle"]["name"];
    scenarios.emplace_back(feature_name + "." + scenario_name);
  }
  return scenarios;
}

template <class TSteps>
inline auto get_scenarios(const std::string& feature) {
  std::vector<std::string> scenarios{};
  if (is_dir(feature.c_str())) {
    auto dp = opendir(feature.c_str());
    assert(dp);
    dirent* entry{};
    while ((entry = readdir(dp))) {
      if (is_file(feature + "/" + entry->d_name)) {
        const auto names = get_feature_scenario<TSteps>(feature + "/" + entry->d_name);
        scenarios.insert(scenarios.end(), names.begin(), names.end());
      }
    }
    closedir(dp);
  } else {
    return get_feature_scenario<TSteps>(feature);
  }

  return scenarios;
}

template <class TSteps>
inline void run_scenario(const std::string& features) {
  for (const auto& feature : split(features, ';')) {
    run_scenarios<TSteps>(feature);
  }
}

template <class TSteps>
inline auto get_scenario(const std::string& features) {
  std::vector<std::string> scenarios{};
  for (const auto& feature : split(features, ';')) {
    const auto names = get_scenarios<TSteps>(feature);
    scenarios.insert(scenarios.end(), names.begin(), names.end());
  }
  return scenarios;
}

}  // detail

template <class TSteps, class... Ts>
inline void RunScenario(const Ts&... features) {
  using swallow = int[];
  (void)swallow{0, (detail::run_scenario<TSteps>(features), 0)...};
}

template <class TSteps, class... Ts>
inline auto GetScenario(const Ts&... features) {
  std::vector<std::string> scenarios{};
  using swallow = int[];
  const auto append = [&scenarios](const auto& v) { scenarios.insert(scenarios.end(), v.begin(), v.end()); };
  (void)swallow{0, (append(detail::get_scenario<TSteps>(features)), 0)...};
  return scenarios;
}

}  // v1
}  // testing

#define GIVEN(regex)                                                                                                           \
  ::testing::self __GUNIT_CAT(self, __LINE__){this};                                                                           \
  ::testing::step<decltype(__GUNIT_CAT(regex, __string)), decltype(__GUNIT_CAT(__FILE__ "", __string)), __LINE__> __GUNIT_CAT( \
      step_, __LINE__)

#define $Given GIVEN

#define WHEN(regex)                                                                                                            \
  ::testing::self __GUNIT_CAT(self, __LINE__){this};                                                                           \
  ::testing::step<decltype(__GUNIT_CAT(regex, __string)), decltype(__GUNIT_CAT(__FILE__ "", __string)), __LINE__> __GUNIT_CAT( \
      step_, __LINE__)

#define $When WHEN

#define THEN(regex)                                                                                                            \
  ::testing::self __GUNIT_CAT(self, __LINE__){this};                                                                           \
  ::testing::step<decltype(__GUNIT_CAT(regex, __string)), decltype(__GUNIT_CAT(__FILE__ "", __string)), __LINE__> __GUNIT_CAT( \
      step_, __LINE__)

#define $Then THEN

#define GSCENARIO(type, ...)                                             \
  class type;                                                            \
  GTEST(#type) {                                                         \
    auto id = 0;                                                         \
    for (const auto& name : ::testing::GetScenario<type>(__VA_ARGS__)) { \
      if (tr_gtest.run("SCENARIO", name, id++)) {                        \
        ::testing::RunScenario<type>(__VA_ARGS__);                       \
      }                                                                  \
    }                                                                    \
  }

#define $GScenario GSCENARIO
