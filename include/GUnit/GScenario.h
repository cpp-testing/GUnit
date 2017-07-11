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
#include <functional>
#include <gherkin.hpp>
#include <json.hpp>
#include <stdexcept>
#include <type_traits>
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

using Table = std::vector<std::unordered_map<std::string, std::string>>;

class Steps;

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

inline auto parse(const std::string& feature, const std::wstring& content) {
  gherkin::parser parser{L"en"};
  gherkin::compiler compiler{feature};
  const auto gherkin_document = parser.parse(content);
  return compiler.compile(gherkin_document);
}

inline auto make_table(const nlohmann::json& step) {
  Table table{};
  std::vector<std::string> ids{};
  for (const auto& argument : step["arguments"]) {
    auto first = true;
    for (const auto& row : argument["rows"]) {
      if (first) {
        for (const auto& cell : row["cells"]) {
          ids.push_back(cell["value"]);
        }
        first = false;
      } else {
        std::unordered_map<std::string, std::string> r;
        int i = 0;
        for (const auto& cell : row["cells"]) {
          std::string value = cell["value"];
          r[ids[i++]] = value;
        }
        table.push_back(r);
      }
    }
  }
  return table;
}

inline void run(
    const std::string& pickles,
    const std::function<void()>& before_each,
    const std::unordered_map<std::string, std::pair<std::string, std::function<void(const std::string&, const Table&)>>>& steps,
    const std::function<void()>& after_each) {
  const auto json = nlohmann::json::parse(pickles)["pickle"];
  for (const auto& expected_step : json["steps"]) {
    std::string line = expected_step["text"];
    const auto table = make_table(expected_step);
    auto found = false;
    for (const auto& given_step : steps) {
      if (detail::match(given_step.first, line)) {
        if (found) {
          throw StepIsAmbiguous{"STEP \"" + line + "\" is ambiguous!"};
        }
        std::cout << "\033[0;96m"
                  << "[ " << std::right << std::setw(8) << given_step.second.first << " ] " << line << "\033[m" << '\n';
        if (before_each) {
          before_each();
        }
        given_step.second.second(line, table);
        if (after_each) {
          after_each();
        }
        found = true;
      }
    }

    if (not found) {
      throw StepIsNotImplemented{"STEP \"" + line + "\" not implemented!"};
    }
  }
}

template<class TSteps, class T, class... Ts>
inline auto call_steps(const TSteps& steps, const std::string& pickles, detail::type_list<T, Ts...>) {
  return steps(T{pickles}, Ts{}...);
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
            static_assert(std::is_same<Steps, decltype(call_steps(steps, pickles, detail::function_args_t<TSteps, Steps>{}))>{},
                          "STEPS implementation has to return testing::Steps type!");
            call_steps(steps, pickles, detail::function_args_t<TSteps, Steps>{});
            std::cout << '\n';
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
    if (scenario) {
      for (const auto& feature : detail::split(scenario, ':')) {
        parse_and_register(TFeature::c_str(), s, feature);
      }
    }
  }
};

template<class T>
inline auto lexical_table_cast(const std::string& str, const Table&, detail::identity<T>) {
  return detail::lexical_cast<T>(str);
}

inline auto lexical_table_cast(const std::string&, const Table& table, detail::identity<const Table&>) {
  return table;
}

inline auto lexical_table_cast(const std::string&, const Table& table, detail::identity<Table>) {
  return table;
}

}  // detail

class Steps {
 public:
  explicit Steps(const std::string& scenario) : scenario_{scenario} {}

  Steps(const Steps& steps) { detail::run(steps.scenario_, steps.before__, steps.steps_, steps.after__); }

  template <class File = detail::string<>, int line = 0, class TPattern>
  auto Given(const TPattern& pattern) {
    constexpr auto size = detail::args_size(TPattern{});
    return step<size>{"Given", pattern.c_str(), steps_[pattern.c_str()]};
  }

  template <class File = detail::string<>, int line = 0, class TPattern, class T>
  auto Given(const TPattern& pattern, const T&) {
    constexpr auto size = detail::args_size(TPattern{});
    return step<size, true>{"Given", pattern.c_str(), steps_[pattern.c_str()]};
  }

  template <class File = detail::string<>, int line = 0>
  auto Given(const char* pattern) {
    return step<>{"Given", pattern, steps_[pattern]};
  }

  template <class File = detail::string<>, int line = 0, class T>
  auto Given(const char* pattern, const T&) {
    return step<-1, true>{"Given", pattern, steps_[pattern]};
  }

  template <class File = detail::string<>, int line = 0, class TPattern>
  auto When(const TPattern& pattern) {
    constexpr auto size = detail::args_size(TPattern{});
    return step<size>{"When", pattern.c_str(), steps_[pattern.c_str()]};
  }

  template <class File = detail::string<>, int line = 0, class TPattern, class T>
  auto When(const TPattern& pattern, const T&) {
    constexpr auto size = detail::args_size(TPattern{});
    return step<size, true>{"When", pattern.c_str(), steps_[pattern.c_str()]};
  }

  template <class File = detail::string<>, int line = 0>
  auto When(const char* pattern) {
    return step<>{"When", pattern, steps_[pattern]};
  }

  template <class File = detail::string<>, int line = 0, class T>
  auto When(const char* pattern, const T&) {
    return step<-1, true>{"When", pattern, steps_[pattern]};
  }

  template <class File = detail::string<>, int line = 0, class TPattern>
  auto Then(const TPattern& pattern) {
    constexpr auto size = detail::args_size(TPattern{});
    return step<size>{"Then", pattern.c_str(), steps_[pattern.c_str()]};
  }

  template <class File = detail::string<>, int line = 0, class TPattern, class T>
  auto Then(const TPattern& pattern, const T&) {
    constexpr auto size = detail::args_size(TPattern{});
    return step<size, true>{"Then", pattern.c_str(), steps_[pattern.c_str()]};
  }

  template <class File = detail::string<>, int line = 0>
  auto Then(const char* pattern) {
    return step<>{"Then", pattern, steps_[pattern]};
  }

  template <class File = detail::string<>, int line = 0, class T>
  auto Then(const char* pattern, const T&) {
    return step<-1, true>{"Then", pattern, steps_[pattern]};
  }

  auto Before() {
    return around{before__};
  }

  auto After() {
    return around{after__};
  }

 private:
  class around {
    public:
      explicit around(std::function<void()>& expr)
        : expr_(expr)
      { }

    template <class TExpr>
    void operator=(const TExpr& expr) {
      expr_ = expr;
    }

    private:
      std::function<void()>& expr_;
  };

  template <int ArgsSize = -1, bool HasTable = false>
  class step {
   public:
    step(const std::string& step_name, const std::string& pattern,
         std::pair<std::string, std::function<void(const std::string&, const Table&)>>& expr)
        : step_name_(step_name), pattern_{pattern}, expr_{expr} {}

    template <class TExpr>
    void operator=(const TExpr& expr) {
      expr_ = {step_name_, [ pattern = pattern_, expr ](const std::string& step, const Table& table){
                               call(expr, step, table, pattern, detail::function_traits_t<TExpr>{});
    }
  };
}

private : template <class TExpr, class... Ts>
          static void
          call(const TExpr& expr, const std::string& step, const Table& table, const std::string& pattern,
               detail::type_list<Ts...> t) {
  static_assert(ArgsSize == -1 || ArgsSize + int(HasTable) == sizeof...(Ts),
                "The number of function parameters don't match the number of arguments specified in the pattern!");
  assert(detail::args_size(pattern) + int(HasTable) == sizeof...(Ts));
  call_impl(expr, detail::matches(pattern, step), table, t, std::make_index_sequence<sizeof...(Ts)>{},
            std::integral_constant<bool, HasTable>{});
}

template <class TExpr, class TMatches, class... Ts, std::size_t... Ns>
static void call_impl(const TExpr& expr, const TMatches& matches, const Table&, detail::type_list<Ts...>,
                      std::index_sequence<Ns...>, std::false_type) {
  expr(detail::lexical_cast<Ts>(matches[Ns].c_str())...);
}

template <class TExpr, class TMatches, class... Ts, std::size_t... Ns>
static void call_impl(const TExpr& expr, const TMatches& matches, const Table& table, detail::type_list<Ts...>,
                      std::index_sequence<Ns...>, std::true_type) {
  expr(detail::lexical_table_cast(matches.empty() ? "" : matches[Ns].c_str(), table, detail::identity<Ts>{})...);
}

std::string step_name_;
std::string pattern_;
std::pair<std::string, std::function<void(const std::string&, const Table&)>>& expr_;
};

private:
std::string scenario_;
std::unordered_map<std::string, std::pair<std::string, std::function<void(const std::string&, const Table&)>>> steps_{};
std::function<void()> before__;
std::function<void()> after__;
};

}  // v1
}  // testing

#define STEPS(feature)                                                                                               \
  static __attribute__((unused))::testing::detail::steps<decltype(__GUNIT_CAT(feature, _gtest_string))> __GUNIT_CAT( \
      _gsteps__, __COUNTER__)

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
#endif

#define $Given Given<decltype(__FILE__ ""_gtest_string), __LINE__>
#define $When When<decltype(__FILE__ ""_gtest_string), __LINE__>
#define $Then Then<decltype(__FILE__ ""_gtest_string), __LINE__>
