//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <fcntl.h>
#include <gtest/gtest.h>
#include <unistd.h>
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

template <class T>
class GTestAutoRegister {
  class ScopedStream {
   public:
    explicit ScopedStream(int fileNumber) : fileNumber(fileNumber) {
      backup = dup(fileNumber);
      restore = open("/dev/null", O_WRONLY);
      dup2(restore, fileNumber);
      close(restore);
    }

    ~ScopedStream() {
      dup2(backup, fileNumber);
      close(backup);
    }

   private:
    int backup = 0;
    int restore = 0;
    int fileNumber = 0;
  };

  class ScopedVisibility {
   public:
    ScopedVisibility() { gmock_ready = false; }
    ~ScopedVisibility() { gmock_ready = true; }

   private:
    ScopedStream stdout = ScopedStream{STDOUT_FILENO};
    ScopedStream stderr = ScopedStream{STDERR_FILENO};
    ScopedStream stdin = ScopedStream{STDIN_FILENO};
  };

 public:
  GTestAutoRegister() {
    ScopedVisibility _;
    // static constexpr auto has_tests = std::is_same<decltype(T{}.test()), void>::value;
    // static_assert(not has_tests, "At least one SHOULD/test is required!");
    T{}.test();
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
}  // detail

template <class T>
class GTest : public detail::GTest<T>, public Test {};
}  // v1
}  // testing

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wreturn-type"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wreturn-type"
#endif

#define GTEST(TYPE)                                                              \
  template <class>                                                               \
  class GTEST;                                                                   \
  template <>                                                                    \
  class GTEST<TYPE> : public ::testing::detail::GTest<TYPE> {                    \
    using TEST_TYPE = GTEST;                                                     \
    static constexpr auto TEST_NAME = #TYPE;                                     \
                                                                                 \
   public:                                                                       \
    auto test();                                                                 \
  };                                                                             \
  ::testing::detail::GTestAutoRegister<GTEST<TYPE>> __GUNIT_CAT(ar, __LINE__){}; \
  auto GTEST<TYPE>::test()

#define SHOULD(NAME)                                                                                                          \
  static auto __GUNIT_CAT(once_, __LINE__) = true;                                                                            \
  const auto __GUNIT_CAT(test_case_name_, __LINE__) = std::string{"should "} + NAME;                                          \
  if (__GUNIT_CAT(once_, __LINE__)) {                                                                                         \
    __GUNIT_CAT(once_, __LINE__) = false;                                                                                     \
    ::testing::internal::MakeAndRegisterTestInfo(TEST_NAME, __GUNIT_CAT(test_case_name_, __LINE__).c_str(), nullptr, nullptr, \
                                                 ::testing::internal::CodeLocation(__FILE__, __LINE__),                       \
                                                 ::testing::internal::GetTestTypeId(), ::testing::Test::SetUpTestCase,        \
                                                 ::testing::Test::TearDownTestCase,                                           \
                                                 new ::testing::detail::GTestFactoryImpl<TEST_TYPE>{});                       \
  } else if (__GUNIT_CAT(test_case_name_, __LINE__) == ::testing::UnitTest::GetInstance()->current_test_info()->name())
