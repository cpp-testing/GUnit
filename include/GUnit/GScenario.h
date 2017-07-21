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
#include "GUnit/Detail/FileUtils.h"
#include "GUnit/Detail/Preprocessor.h"
#include "GUnit/Detail/RegexUtils.h"
#include "GUnit/Detail/StringUtils.h"
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

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif

template <class T, T... Chrs>
constexpr auto operator""_step() {
  return detail::string<Chrs...>{};
}

namespace detail {

struct step_info {
  std::string name{};
  std::string file{};
  int line{};
};

using call_step_t = std::function<void(const std::string&, const Table&)>;
using step_info_call_step_t = std::pair<step_info, call_step_t>;
using step_info_call_map_t = std::unordered_map<std::string, step_info_call_step_t>;

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

inline void run(const std::string& feature_file, const std::string& pickles, const std::function<void()>& before,
                const step_info_call_map_t& steps, const std::function<void()>& after) {
  const auto json = nlohmann::json::parse(pickles)["pickle"];
  for (const auto& expected_step : json["steps"]) {
    std::string text = expected_step["text"];
    auto found = false;
    for (const auto& given_step : steps) {
      if (detail::match(given_step.first, text)) {
        if (found) {
          throw StepIsAmbiguous{"STEP \"" + text + "\" is ambiguous!"};
        }
        const auto name = given_step.second.first.name;
        const auto full_file = given_step.second.first.file.empty() ? feature_file : given_step.second.first.file;
        const auto file = full_file.substr(full_file.find_last_of("/\\") + 1);
        const auto line = not given_step.second.first.line ? expected_step["locations"].back()["line"].get<int>()
                                                           : given_step.second.first.line;

        std::cout << "\033[0;96m"
                  << "[ " << std::right << std::setw(8) << name << " ] " << std::left << std::setw(60) << text << "# " << file
                  << ":" << line << "\033[m" << '\n';
        if (before) {
          before();
        }
        given_step.second.second(text, make_table(expected_step));
        if (after) {
          after();
        }
        found = true;
      }
    }

    if (not found) {
      throw StepIsNotImplemented{"STEP \"" + text + "\" not implemented!"};
    }
  }
}

template <class TSteps, class T, class... Ts>
inline auto call_steps(const TSteps& steps, const std::string& pickles, const std::string& file, detail::type_list<T, Ts...>) {
  return steps(T{file, pickles}, Ts{}...);
}

inline std::pair<bool, std::string> make_tags(const nlohmann::json& tags) {
  if (tags.empty()) {
    return {};
  }

  std::string result{"["};
  auto disabled = false;
  auto i = 0;
  for (const auto& tag : tags) {
    std::string tag_name = tag["name"];
    if (i++) {
      result += ",";
    }
    if (tag_name == "@disabled") {
      disabled = true;
    } else {
      result += tag_name;
    }
  }
  result += "]";
  return {disabled, result};
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
    const auto pickle_json = nlohmann::json::parse(pickle)["pickle"];
    const std::string scenario_name = pickle_json["name"];
    const auto tags = make_tags(pickle_json["tags"]);
    const auto disabled = tags.first ? "DISABLED_" : "";

    if (PatternMatchesString(name.c_str(), feature_name.c_str())) {
      class TestFactory : public internal::TestFactoryBase {
        class test : public Test {
         public:
          test(const TSteps& steps, const std::string& pickles, const std::string& file)
              : steps{steps}, pickles{pickles}, file{file} {}

          void TestBody() {
            static_assert(
                std::is_same<Steps, decltype(call_steps(steps, pickles, file, detail::function_args_t<TSteps, Steps>{}))>{},
                "STEPS implementation has to return testing::Steps type!");
            call_steps(steps, pickles, file, detail::function_args_t<TSteps, Steps>{});
            std::cout << '\n';
          }

         private:
          TSteps steps;
          std::string pickles;
          std::string file;
        };

       public:
        TestFactory(const TSteps& steps, const std::string& pickles, const std::string& file)
            : steps{steps}, pickles{pickles}, file{file} {}
        Test* CreateTest() override { return new test{steps, pickles, file}; }

       private:
        TSteps steps;
        std::string pickles;
        std::string file;
      };

      MakeAndRegisterTestInfo(new TestFactory{steps, pickle, feature}, disabled + feature_name + tags.second, scenario_name,
                              __FILE__, __LINE__, detail::type<decltype(internal::MakeAndRegisterTestInfo)>{});
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

template <class T>
inline auto lexical_table_cast(const std::string& str, const Table&, detail::identity<T>) {
  return detail::lexical_cast<T>(str);
}
inline auto lexical_table_cast(const std::string&, const Table& table, detail::identity<const Table&>) { return table; }
inline auto lexical_table_cast(const std::string&, const Table& table, detail::identity<Table>) { return table; }

}  // detail

class Steps {
 public:
  explicit Steps(const std::string& file, const std::string& scenario) : file_{file}, scenario_{scenario} {}

  Steps(const Steps& steps) { detail::run(steps.file_, steps.scenario_, steps.before_, steps.steps_, steps.after_); }

  template <class File = detail::string<>, int line = 0, class TPattern>
  auto Given(const TPattern& pattern) {
    constexpr auto size = detail::args_size(TPattern{});
    return step<size>{{"Given", File::c_str(), line}, pattern.c_str(), steps_[pattern.c_str()]};
  }

  template <class File = detail::string<>, int line = 0, class TPattern, class T>
  auto Given(const TPattern& pattern, const T&) {
    constexpr auto size = detail::args_size(TPattern{});
    return step<size, true>{{"Given", File::c_str(), line}, pattern.c_str(), steps_[pattern.c_str()]};
  }

  template <class File = detail::string<>, int line = 0>
  auto Given(const char* pattern) {
    return step<>{{"Given", File::c_str(), line}, pattern, steps_[pattern]};
  }

  template <class File = detail::string<>, int line = 0, class T>
  auto Given(const char* pattern, const T&) {
    return step<-1, true>{{"Given", File::c_str(), line}, pattern, steps_[pattern]};
  }

  template <class File = detail::string<>, int line = 0, class TPattern>
  auto When(const TPattern& pattern) {
    constexpr auto size = detail::args_size(TPattern{});
    return step<size>{{"When", File::c_str(), line}, pattern.c_str(), steps_[pattern.c_str()]};
  }

  template <class File = detail::string<>, int line = 0, class TPattern, class T>
  auto When(const TPattern& pattern, const T&) {
    constexpr auto size = detail::args_size(TPattern{});
    return step<size, true>{{"When", File::c_str(), line}, pattern.c_str(), steps_[pattern.c_str()]};
  }

  template <class File = detail::string<>, int line = 0>
  auto When(const char* pattern) {
    return step<>{{"When", File::c_str(), line}, pattern, steps_[pattern]};
  }

  template <class File = detail::string<>, int line = 0, class T>
  auto When(const char* pattern, const T&) {
    return step<-1, true>{{"When", File::c_str(), line}, pattern, steps_[pattern]};
  }

  template <class File = detail::string<>, int line = 0, class TPattern>
  auto Then(const TPattern& pattern) {
    constexpr auto size = detail::args_size(TPattern{});
    return step<size>{{"Then", File::c_str(), line}, pattern.c_str(), steps_[pattern.c_str()]};
  }

  template <class File = detail::string<>, int line = 0, class TPattern, class T>
  auto Then(const TPattern& pattern, const T&) {
    constexpr auto size = detail::args_size(TPattern{});
    return step<size, true>{{"Then", File::c_str(), line}, pattern.c_str(), steps_[pattern.c_str()]};
  }

  template <class File = detail::string<>, int line = 0>
  auto Then(const char* pattern) {
    return step<>{{"Then", File::c_str(), line}, pattern, steps_[pattern]};
  }

  template <class File = detail::string<>, int line = 0, class T>
  auto Then(const char* pattern, const T&) {
    return step<-1, true>{{"Then", File::c_str(), line}, pattern, steps_[pattern]};
  }

  auto Before() { return around{before_}; }
  auto After() { return around{after_}; }

 private:
  class around {
   public:
    explicit around(std::function<void()>& expr) : expr_(expr) {}

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
    step(const detail::step_info& step_info, const std::string& pattern, detail::step_info_call_step_t& expr)
        : step_info_(step_info), pattern_{pattern}, expr_{expr} {}

    template <class TExpr>
    void operator=(const TExpr& expr) {
      expr_ = {step_info_, [ pattern = pattern_, expr ](const std::string& step, const Table& table){
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
  expr(detail::lexical_table_cast(Ns < matches.size() ? matches[Ns].c_str() : "", table, detail::identity<Ts>{})...);
}

detail::step_info step_info_;
std::string pattern_;
detail::step_info_call_step_t& expr_;
};

private:
std::string file_;
std::string scenario_;
detail::step_info_call_map_t steps_{};
std::function<void()> before_;
std::function<void()> after_;
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
