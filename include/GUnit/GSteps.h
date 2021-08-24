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
#include <cstdlib>
#include <functional>
#include <gherkin.hpp>
#include <json.hpp>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "GUnit/Detail/FileUtils.h"
#include "GUnit/Detail/Preprocessor.h"
#include "GUnit/Detail/RegexUtils.h"
#include "GUnit/Detail/StringUtils.h"
#include "GUnit/Detail/Utility.h"
#include "formatters/features.hpp"
#include "formatters/gherkinCpp/background.hpp"
#include "formatters/gherkinCpp/element.hpp"
#include "formatters/gherkinCpp/feature.hpp"
#include "formatters/gherkinCpp/scenario.hpp"
#include "formatters/gherkinCpp/scenariooutline.hpp"
#include "formatters/gherkinCpp/step.hpp"
#include "formatters/gherkinCpp/tag.hpp"

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
struct Items : std::vector<T> {
  using std::vector<T>::operator[];

  decltype(auto) operator[](const std::string& id) const {
    assert(1 == this->size());
    static typename T::mapped_type default_value{};
    return (this->at(0).count(id)) ? this->at(0).at(id) : default_value;
  }

  std::string text{};
};

template <class T>
class Convertible : public T {
 public:
  Convertible() = default;
  Convertible(const T& value) : T{value}, available_{true} {}

  template <class U, class = std::enable_if_t<std::is_integral<U>::value ||
                                              std::is_floating_point<U>::value>>
  operator U() const {
    return lexical_cast<U>(*this);
  }

  auto available() const { return available_; }

 private:
  bool available_{};
};

}  // namespace detail

using Table = detail::Items<
    std::unordered_map<std::string, detail::Convertible<std::string>>>;

using Data = Table;

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

template <class T, T... Chrs>
constexpr auto operator""_step() {
  return detail::string<Chrs...>{};
}

namespace detail {
template <class T>
void MakeAndRegisterTestInfo(
    const T& test, const std::string& type, const std::string& name,
    const std::string& /*file*/, int /*line*/,
    detail::type<TestInfo*(const char*, const char*, const char*, const char*,
                           const void*, void (*)(), void (*)(),
                           internal::TestFactoryBase*)>) {
  internal::MakeAndRegisterTestInfo(
      type.c_str(), name.c_str(), nullptr, nullptr, internal::GetTestTypeId(),
      Test::SetUpTestCase, Test::TearDownTestCase, test);
}

template <class T, class... Ts>
void MakeAndRegisterTestInfo(const T& test, const std::string& type,
                             const std::string& name, const std::string& file,
                             int line, detail::type<TestInfo*(Ts...)>) {
  internal::MakeAndRegisterTestInfo(
      type.c_str(), name.c_str(), nullptr, nullptr, {file.c_str(), line},
      internal::GetTestTypeId(), Test::SetUpTestCase, Test::TearDownTestCase,
      test);
}

inline auto make_table(const nlohmann::json& step) {
  Table table{};
  std::vector<std::string> ids{};
  for (const auto& argument : step["arguments"]) {
    if (argument.find("content") != argument.end()) {
      const std::string content = argument["content"];
      table.text = content;
      return table;
    }

    auto first = true;
    for (const auto& row : argument["rows"]) {
      if (first) {
        for (const auto& cell : row["cells"]) {
          ids.push_back(cell["value"]);
        }
        first = false;
      } else {
        using row_t = typename Table::value_type;
        row_t r{};
        auto i = 0;
        for (const auto& cell : row["cells"]) {
          const std::string value = cell["value"];
          r[ids[i++]] = typename row_t::mapped_type{value};
        }
        table.push_back(r);
      }
    }
  }
  return table;
}

template <class T>
static inline auto lexical_table_cast(const std::string& str, const Table&,
                                      detail::identity<T>) {
  return detail::lexical_cast<T>(str);
}
static inline auto lexical_table_cast(const std::string&, const Table& table,
                                      detail::identity<const Table&>) {
  return table;
}
static inline auto lexical_table_cast(const std::string&, const Table& table,
                                      detail::identity<Table>) {
  return table;
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

template <class TFeature, class TCRTP>
class Steps : public ::testing::EmptyTestEventListener {
  class TestFactory : public internal::TestFactoryBase {
    class TestImpl : public Test {
     public:
      TestImpl(Steps& steps, const std::string& pickles,
               const std::string& file,
               std::shared_ptr<GherkinCpp::Element> element)
          : steps{steps}, pickles{pickles}, file{file}, element{element} {}

      void TestBody() {
        steps.Init(pickles, file, element);
        static_cast<TCRTP&>(steps).Run();
        if (steps.current_step_ != steps.pickle_steps_.size()) {
          if (steps.currentStep != nullptr) steps.currentStep->setResult(false);
          throw StepIsNotImplemented{"STEP \"" + steps.not_found_ +
                                     "\" not implemented!"};
        }
        std::cout << '\n';
      }

     private:
      Steps& steps;
      std::string pickles;
      std::string file;
      std::shared_ptr<GherkinCpp::Element> element;
    };

   public:
    TestFactory(Steps& steps, const std::string& pickles,
                const std::string& file,
                std::shared_ptr<GherkinCpp::Element> element)
        : steps{steps}, pickles{pickles}, file{file}, element{element} {}
    Test* CreateTest() override {
      return new TestImpl{steps, pickles, file, element};
    }

   private:
    Steps& steps;
    std::string pickles;
    std::string file;
    std::shared_ptr<GherkinCpp::Element> element;
  };

  struct StepInfo {
    std::string name{};
    std::string file{};
    int line{};
  };

  using CallStep_t = std::function<void(const std::string&, const Table&)>;
  using StepInfoCallStep_t = std::pair<StepInfo, CallStep_t>;
  using StepInfoCalls_t = std::unordered_map<std::string, StepInfoCallStep_t>;

  template <int ArgsSize = -1, bool HasTable = false>
  class Step {
   public:
    Step(Steps& steps, const StepInfo& StepInfo, const std::string& pattern)
        : steps_{steps},
          StepInfo_(StepInfo),
          pattern_{pattern},
          expr_{steps.steps_[pattern]} {}

    ~Step() { steps_.NextStep(); }

    template <class TExpr>
    void operator=(const TExpr& expr) {
      const auto pattern = pattern_;
      expr_ = {StepInfo_,
               [expr, pattern](const std::string& step, const Table& table) {
                 Call(expr, step, table, pattern,
                      detail::function_traits_t<TExpr>{});
               }};
    }

   private:
    template <class TExpr, class... Ts>
    static void Call(const TExpr& expr, const std::string& step,
                     const Table& table, const std::string& pattern,
                     detail::type_list<Ts...> t) {
      static_assert(
          ArgsSize == -1 || ArgsSize + int(HasTable) == sizeof...(Ts),
          "The number of function parameters don't match the number of "
          "arguments specified in the pattern!");
      assert(detail::args_size(pattern) + int(HasTable) == sizeof...(Ts));
      CallImpl(expr, detail::matches(pattern, step), table, t,
               std::make_index_sequence<sizeof...(Ts)>{},
               std::integral_constant<bool, HasTable>{});
    }

    template <class TExpr, class TMatches, class... Ts, std::size_t... Ns>
    static void CallImpl(const TExpr& expr, const TMatches& matches,
                         const Table&, detail::type_list<Ts...>,
                         std::index_sequence<Ns...>, std::false_type) {
      expr(detail::lexical_cast<Ts>(matches[Ns].c_str())...);
    }

    template <class TExpr, class TMatches, class... Ts, std::size_t... Ns>
    static void CallImpl(const TExpr& expr, const TMatches& matches,
                         const Table& table, detail::type_list<Ts...>,
                         std::index_sequence<Ns...>, std::true_type) {
      expr(lexical_table_cast(Ns < matches.size() ? matches[Ns].c_str() : "",
                              table, detail::identity<Ts>{})...);
    }

    Steps& steps_;
    StepInfo StepInfo_;
    std::string pattern_;
    StepInfoCallStep_t& expr_;
  };

 public:
  Steps() {
    const auto scenario = std::getenv("SCENARIO");
    const auto output = std::getenv("OUTPUT");
    if (scenario) {
      ::testing::UnitTest::GetInstance()->listeners().Append(this);
      // If the output is set, then add a report to the features holder
      if (output) {
        Features::getInstance()->addReport("gunit_result");
      }
      for (const auto& feature : detail::split(scenario, ':')) {
        info_.file = feature;
        ParseAndRegister(TFeature::c_str(), feature);
      }
    }
  }
  ~Steps() { ::testing::UnitTest::GetInstance()->listeners().Release(this); }

  template <class File = detail::string<>, int line = 0, class TPattern>
  auto Given(const TPattern& pattern) {
    constexpr auto size = detail::args_size(TPattern{});
    return Step<size>{*this, {"Given", File::c_str(), line}, pattern.c_str()};
  }

  template <class File = detail::string<>, int line = 0, class TPattern,
            class T>
  auto Given(const TPattern& pattern, const T&) {
    constexpr auto size = detail::args_size(TPattern{});
    return Step<size, true>{
        *this, {"Given", File::c_str(), line}, pattern.c_str()};
  }

  template <class File = detail::string<>, int line = 0>
  auto Given(const char* pattern) {
    return Step<>{*this, {"Given", File::c_str(), line}, pattern};
  }

  template <class File = detail::string<>, int line = 0, class T>
  auto Given(const char* pattern, const T&) {
    return Step<-1, true>{*this, {"Given", File::c_str(), line}, pattern};
  }

  template <class File = detail::string<>, int line = 0, class TPattern>
  auto When(const TPattern& pattern) {
    constexpr auto size = detail::args_size(TPattern{});
    return Step<size>{*this, {"When", File::c_str(), line}, pattern.c_str()};
  }

  template <class File = detail::string<>, int line = 0, class TPattern,
            class T>
  auto When(const TPattern& pattern, const T&) {
    constexpr auto size = detail::args_size(TPattern{});
    return Step<size, true>{
        *this, {"When", File::c_str(), line}, pattern.c_str()};
  }

  template <class File = detail::string<>, int line = 0>
  auto When(const char* pattern) {
    return Step<>{*this, {"When", File::c_str(), line}, pattern};
  }

  template <class File = detail::string<>, int line = 0, class T>
  auto When(const char* pattern, const T&) {
    return Step<-1, true>{*this, {"When", File::c_str(), line}, pattern};
  }

  template <class File = detail::string<>, int line = 0, class TPattern>
  auto Then(const TPattern& pattern) {
    constexpr auto size = detail::args_size(TPattern{});
    return Step<size>{*this, {"Then", File::c_str(), line}, pattern.c_str()};
  }

  template <class File = detail::string<>, int line = 0, class TPattern,
            class T>
  auto Then(const TPattern& pattern, const T&) {
    constexpr auto size = detail::args_size(TPattern{});
    return Step<size, true>{
        *this, {"Then", File::c_str(), line}, pattern.c_str()};
  }

  template <class File = detail::string<>, int line = 0>
  auto Then(const char* pattern) {
    return Step<>{*this, {"Then", File::c_str(), line}, pattern};
  }

  template <class File = detail::string<>, int line = 0, class T>
  auto Then(const char* pattern, const T&) {
    return Step<-1, true>{*this, {"Then", File::c_str(), line}, pattern};
  }

// clang-format off
  #if defined(__clang__)
  #pragma clang diagnostic ignored "-Wdollar-in-identifier-extension"
  #endif

  #define $Given Given<decltype(__FILE__ ""_gtest_string), __LINE__>
  #define $When When<decltype(__FILE__ ""_gtest_string), __LINE__>
  #define $Then Then<decltype(__FILE__ ""_gtest_string), __LINE__>
  // clang-format on

  const auto& Info() const { return info_; }

 private:
  void Init(const std::string& pickles, const std::string& file,
            std::shared_ptr<GherkinCpp::Element> element) {
    info_.scenario =
        ::testing::UnitTest::GetInstance()->current_test_info()->name();
    steps_ = {};
    current_step_ = {};
    pickle_steps_ = nlohmann::json::parse(pickles)["pickle"]["steps"];
    not_found_ = {};
    file_ = file;
    currentElement = element;
  }

  void ParseAndRegister(const std::string& name, const std::string& feature) {
    try {
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
        const auto tags = detail::make_tags(pickle_json["tags"]);
        const auto disabled = tags.first ? "DISABLED_" : "";
        const auto full_name = feature_name + "." + scenario_name;

        if (PatternMatchesString(name.c_str(), full_name.c_str())) {
          info_.feature = feature_name;

          //---------------------------- OBJECT CONVERSION -----------------
          // part that parses the pickle and ast files and creates the
          // Gherkin-Cpp objects
          // Check if current feature already exists. If not, creates it.
          std::string tagnames;
          for (const auto& tag : ast["document"]["feature"]["tags"]) {
            tagnames += tag["name"];
          }
          const std::string featureKey = feature_name + tagnames;
          std::shared_ptr<GherkinCpp::Feature> curFeature =
              Features::getInstance()->getFeature(featureKey);
          // if the feature does not exist yet, then it is created and added to
          // the features list.
          if (curFeature == nullptr) {
            int featureLine = ast["document"]["feature"]["location"]["line"];
            curFeature = std::make_shared<GherkinCpp::Feature>(
                feature_name, feature, featureLine);

            // add the tags to the feature
            for (const auto& tag : ast["document"]["feature"]["tags"]) {
              std::shared_ptr<GherkinCpp::Tag> newTag =
                  std::make_shared<GherkinCpp::Tag>(tag["name"],
                                                    tag["location"]["line"]);
              curFeature->addTag(std::move(newTag));
            }
            Features::getInstance()->addFeature(curFeature);
          }

          // Scenario outlines will have two locations because they have the
          // Examples. When this happens, the first is the line of the example,
          // and the second, the one from its declaration. because we want to
          // have multiple scenario outlines containing the replaced values in
          // the feature, we take the line from the example
          int currentScenarioLine = pickle_json["locations"].front()["line"];
          std::string currentScenarioName = pickle_json["name"];
          std::string elementKey =
              currentScenarioName + " " + std::to_string(currentScenarioLine);

          std::shared_ptr<GherkinCpp::Element> element =
              curFeature->getSpecificElement(elementKey);
          // if the element does not exist yet, it is added to the feature
          if (element == nullptr) {
            // Loop through children (ast file) to find which type matches the
            // current pickle name.
            for (const auto& children :
                 ast["document"]["feature"]["children"]) {
              // store keyword and name
              std::string keyword = children["keyword"];
              std::string name = children["name"];
              // check if the name of the pickle and children matches
              if (name.compare(pickle_json["name"]) == 0) {
                // Check what is its keyword
                if (keyword.compare("Scenario") == 0) {
                  std::shared_ptr<GherkinCpp::Scenario> newScenario =
                      std::make_shared<GherkinCpp::Scenario>(
                          name, currentScenarioLine);
                  element = newScenario;
                } else if (keyword.compare("Scenario Outline") == 0) {
                  std::shared_ptr<GherkinCpp::ScenarioOutline>
                      newScenarioOutline =
                          std::make_shared<GherkinCpp::ScenarioOutline>(
                              name, currentScenarioLine);
                  element = newScenarioOutline;
                }
                curFeature->addElement(element);

                if (children.find("tags") != children.end()) {
                  for (const auto& tag : children["tags"]) {
                    std::shared_ptr<GherkinCpp::Tag> newTag =
                        std::make_shared<GherkinCpp::Tag>(
                            tag["name"], tag["location"]["line"]);
                    element->addTag(std::move(newTag));
                  }
                }
                // as soon as the element is created, break the loop
                break;
              } /* if(name.compare(pickle_json["name"]) */
            }
            // Loop through children again and add the steps.
            for (const auto& children :
                 ast["document"]["feature"]["children"]) {
              // Even if the name does not match, store the step in an
              if (children.find("steps") != children.end()) {
                // loop through the steps of the children (ast file)
                for (const auto& step : children["steps"]) {
                  // loop through the steps of the pickle
                  for (const auto& steps : pickle_json["steps"]) {
                    // if the location of the steps matches, it means that they
                    // are the same
                    if (steps["locations"].back()["line"] ==
                        step["location"]["line"]) {
                      std::shared_ptr<GherkinCpp::Step> newStep =
                          std::make_shared<GherkinCpp::Step>(
                              step["keyword"], steps["text"],
                              step["location"]["line"]);
                      element->addStep(std::move(newStep));
                    }
                  }
                }
              }
            }
          }

          // Registers the test in Google Test
          detail::MakeAndRegisterTestInfo(
              new TestFactory{*this, pickle, feature, element},
              disabled + feature_name + tags.second, scenario_name, __FILE__,
              __LINE__,
              detail::type<decltype(internal::MakeAndRegisterTestInfo)>{});
        }
      }
    } catch (const gherkin::v1::parsing_error& err) {
      const auto err_json = nlohmann::json::parse(err.what());
      const auto line = err_json["source"]["start"]["line"];
      const auto column = err_json["source"]["start"]["column"];
      const std::string error = err_json["data"];

      std::cerr << feature << ":" << line << ":" << column
                << ": error: " << error << std::endl;
      std::exit(-1);
    }
  }

  void NextStep() {
    auto i = 0u;
    if (currentElement == nullptr) return;
    for (const auto& expectedStep : currentElement->getSteps()) {
      // Set the current step as the expected step.
      currentStep = expectedStep.second;

      // ------------------------
      // Iterate through pickle_steps, because detail::make_table expects a json
      // with the step to be executed. need to investigate if we can remove this
      // part
      nlohmann::json expected_step{};
      for (const auto& exp_step : pickle_steps_) {
        if (exp_step["text"] == expectedStep.second->name) {
          if (exp_step["text"] == expectedStep.second->name) {
            expected_step = exp_step;
            break;
          }
        }
      }
      //---------------------------
      // From original code. This is done so it is known at which point of the
      // loop it has stopped. Maybe a refactor to use a forward list here would
      // be better, but then we must restartd the for (const auto& given_step :
      // steps_) loop and also find a strategy to see if the test was not
      // implemented.
      /// todo: remove the current_step_
      if (i++ == current_step_) {
        auto found = false;
        for (const auto& given_step : steps_) {
          if (detail::match(given_step.first, expectedStep.second->name)) {
            if (found) {
              expectedStep.second->setResult(false);
              throw StepIsAmbiguous{"STEP \"" + expectedStep.second->name +
                                    "\" is ambiguous!"};
            }
            // update the curret step
            current_step_ = i;
            const auto name = given_step.second.first.name;
            const auto full_file = given_step.second.first.file.empty()
                                       ? file_
                                       : given_step.second.first.file;
            const auto file =
                full_file.substr(full_file.find_last_of("/\\") + 1);
            const auto line = expectedStep.second->line;

            std::cout << "\033[0;96m"
                      << "[ " << std::right << std::setw(8) << name << " ] "
                      << std::left << std::setw(60) << expectedStep.second->name
                      << "# " << file << ":" << line << "\033[m" << '\n';
            info_.step = expectedStep.second->name;
            given_step.second.second(expectedStep.second->name,
                                     detail::make_table(expected_step));
            found = true;
          }
        }

        if (not found) {
          not_found_ = expectedStep.second->name;
        }
      } /* (i++ == current_step_) */
    }   /* for(const auto& expectedStep : currentElement->getSteps()) */
  }     /*  NextStep() */

  // Called after a failed assertion.
  virtual void OnTestPartResult(
      const ::testing::TestPartResult& test_part_result) {
    ;
    if (currentStep == nullptr) return;
    currentStep->setResult(!(test_part_result.failed()));
  }

  std::shared_ptr<GherkinCpp::Step>
      currentStep;  ///< Holds the pointer to the current step
  StepInfoCalls_t steps_{};
  std::size_t current_step_{};
  nlohmann::json pickle_steps_{};
  std::string not_found_{};
  std::string file_{};
  std::shared_ptr<GherkinCpp::Element>
      currentElement;  ///< Holds the pointer of the element in this test suite

  struct Info {
    std::string file{};
    std::string feature{};
    std::string scenario{};
    std::string step{};

    friend std::ostream& operator<<(std::ostream& os, const Info& info) {
      os << "File: " << info.file << '\n'
         << "Feature: " << info.feature << '\n'
         << "Scenario: " << info.scenario << '\n'
         << "Step: " << info.step << '\n';
      return os;
    }
  } info_;
};
}  // namespace detail
}  // namespace v1
}  // namespace testing

#define GSTEPS(feature)                                                   \
  template <class, class, int>                                            \
  struct GSTEPS;                                                          \
  template <>                                                             \
  struct GSTEPS<decltype(__GUNIT_CAT(feature, _gtest_string)),            \
                decltype(__GUNIT_CAT(__FILE__, _gtest_string)), __LINE__> \
      : ::testing::detail::Steps<                                         \
            decltype(__GUNIT_CAT(feature, _gtest_string)),                \
            GSTEPS<decltype(__GUNIT_CAT(feature, _gtest_string)),         \
                   decltype(__GUNIT_CAT(__FILE__, _gtest_string)),        \
                   __LINE__>> {                                           \
    void Run();                                                           \
  };                                                                      \
  static __attribute__((unused))                                          \
      GSTEPS<decltype(__GUNIT_CAT(feature, _gtest_string)),               \
             decltype(__GUNIT_CAT(__FILE__, _gtest_string)), __LINE__>    \
          __GUNIT_CAT(_gsteps__, __COUNTER__);                            \
  void GSTEPS<decltype(__GUNIT_CAT(feature, _gtest_string)),              \
              decltype(__GUNIT_CAT(__FILE__, _gtest_string)), __LINE__>::Run()
