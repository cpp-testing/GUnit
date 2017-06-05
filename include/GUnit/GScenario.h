//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <regex>
#include <fstream>
#include <cstring>
#include <cassert>
#include <unordered_map>
#include <utility>
#include <functional>
#include <json.hpp>
#include <gherkin.hpp>
#include <boost/lexical_cast.hpp>
#include "GUnit/Detail/Preprocessor.h"
#include "GUnit/Detail/Utility.h"

#define GIVEN(regex) ::testing::self __GUNIT_CAT(self, __LINE__){this}; ::testing::step<decltype(__GUNIT_CAT(regex, __string)), decltype(__GUNIT_CAT(__FILE__"", __string)), __LINE__> __GUNIT_CAT(step_, __LINE__)
#define WHEN(regex)  ::testing::self __GUNIT_CAT(self, __LINE__){this}; ::testing::step<decltype(__GUNIT_CAT(regex, __string)), decltype(__GUNIT_CAT(__FILE__"", __string)), __LINE__> __GUNIT_CAT(step_, __LINE__)
#define THEN(regex)  ::testing::self __GUNIT_CAT(self, __LINE__){this}; ::testing::step<decltype(__GUNIT_CAT(regex, __string)), decltype(__GUNIT_CAT(__FILE__"", __string)), __LINE__> __GUNIT_CAT(step_, __LINE__)

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
  explicit self(void* ptr) {
    *self_ptr() = ptr;
  }

  static void** self_ptr() {
    static void* ptr{};
    return &ptr;
  }
};

template<class TRegex, class File, int Line>
class step {
 public:
  template<class TExpr>
  step(const TExpr& expr) { // non explicit
    steps::get()[*self::self_ptr()][TRegex::c_str()] = [=](const std::string& st) {
      call(expr, st, TRegex::c_str(), detail::function_traits_t<TExpr>{});
    };
  }

 private:
  template<class TExpr, class... Ts>
  void call(const TExpr& expr, const std::string& step, const std::string& regex, detail::type_list<Ts...> t) {
    std::regex pieces_regex{step};
    std::smatch pieces_match;
    assert(std::regex_match(step, pieces_match, std::regex{regex}));
    using ft_t = detail::function_traits<TExpr>;
    call_impl(expr, pieces_match, detail::type<typename ft_t::base_type>{}, typename ft_t::is_lambda_expr{}, t, std::make_index_sequence<sizeof...(Ts)>{});
  }

  template<class TExpr, class TMatches, class T, class... Ts, std::size_t... Ns>
  void call_impl(const TExpr& expr, const TMatches& matches, detail::type<T>, std::false_type, detail::type_list<Ts...>, std::index_sequence<Ns...>) {
    ((static_cast<T*>(*self::self_ptr()))->*expr)(boost::lexical_cast<Ts>(matches[Ns+1].str().c_str())...);
  }

  template<class TExpr, class TMatches, class T, class... Ts, std::size_t... Ns>
  void call_impl(const TExpr& expr, const TMatches& matches, detail::type<T>, std::true_type, detail::type_list<Ts...>, std::index_sequence<Ns...>) {
    expr(boost::lexical_cast<Ts>(matches[Ns+1].str().c_str())...);
  }
};

template<class TSteps>
class Scenario {
public:
  explicit Scenario(const std::string& features) {
    for (const auto& feature : detail::split(features, ';')) {
      parse_and_run(feature);
    }
  }

private:
  void parse_and_run(const std::string& feature) {
    const auto content = read_file(feature);
    const auto pickles = parse(feature, content);
    run(feature, pickles);
  }

  void run(const std::string& feature, const std::vector<std::string>& pickles) {
    for (const auto &pickle : pickles) {
      run_pickle(feature, nlohmann::json::parse(pickle)["pickle"]);
    }
  }

  template<class TJson>
  void run_pickle(const std::string& feature, const TJson& json) {
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

  std::wstring read_file(const std::string& feature) {
    std::ifstream file{feature};
    assert(file.good());
    return {(std::istreambuf_iterator<char>(file)),
                                std::istreambuf_iterator<char>()};
  }

  std::vector<std::string> parse(const std::string& feature, const std::wstring& content) {
    gherkin::parser parser{L"en"};
    gherkin::compiler compiler{feature};
    const auto gherkin_document = parser.parse(content);
    return compiler.compile(gherkin_document);
  }
};

} // v1
} // testing
