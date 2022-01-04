//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <gtest/gtest.h>

#include <string>

#include "GUnit/Detail/StringUtils.h"

namespace testing {
inline namespace v1 {
namespace detail {

struct info {
  const char* file{};
  unsigned long line{};
  std::string expr{};
  TestPartResult::Type failure{};
};

template <class TShouldError, class TLhs, class TRhs,
          AssertionResult (*Comp)(const char*, const char*, TLhs, TRhs)>
class msg : public decltype(Message()) {
 public:
  msg(const info& info, const std::string& comp, TLhs lhs, TRhs rhs)
      : info_{info},
        comp_{comp},
        lhs_{lhs},
        rhs_{rhs},
        result_{Comp("", "", lhs_, rhs_)} {}
  ~msg() {
    if (TShouldError::value) {
      const auto begin = info_.expr.find(comp_);
      auto lhs_expr = info_.expr.substr(0, begin);
      trim(lhs_expr);
      auto rhs_expr = info_.expr.substr(begin + comp_.size());
      trim(rhs_expr);
      const AssertionResult gtest_ar =
          (Comp(lhs_expr.c_str(), rhs_expr.c_str(), lhs_, rhs_));
      if (!gtest_ar) {
        internal::AssertHelper(info_.failure, info_.file, info_.line,
                               gtest_ar.failure_message()) = *this;
      }
    }
  }

  operator bool() const { return result_; }

 private:
  info info_{};
  std::string comp_{};
  TLhs lhs_;
  TRhs rhs_;
  bool result_{false};
};

template <class TShouldError>
class op {
  template <class TLhs>
  class comp : public decltype(Message()) {
   public:
    explicit comp(const info& info, const TLhs& lhs) : info_{info}, lhs_{lhs} {
      set_result(lhs_);
    }

    ~comp() { assert_error(lhs_); }

    template <class TRhs,
              std::enable_if_t<std::is_floating_point<TLhs>::value ||
                                   std::is_floating_point<TRhs>::value,
                               int> = 0>
    auto operator==(const TRhs& rhs) const {
      followed_ = true;
      return msg<TShouldError, TLhs, TRhs,
                 internal::CmpHelperFloatingPointEQ<TLhs>>{info_, "==", lhs_,
                                                           rhs};
    }

    template <class TRhs,
              std::enable_if_t<!std::is_floating_point<TLhs>::value &&
                                   !std::is_floating_point<TRhs>::value,
                               int> = 0>
    auto operator==(const TRhs& rhs) const {
      followed_ = true;
      return msg<TShouldError, const TLhs&, const TRhs&, internal::CmpHelperEQ>{
          info_, "==", lhs_, rhs};
    }

    template <class TRhs>
    auto operator!=(const TRhs& rhs) const {
      followed_ = true;
      return msg<TShouldError, const TLhs&, const TRhs&, internal::CmpHelperNE>{
          info_, "!=", lhs_, rhs};
    }

    template <class TRhs>
    auto operator>(const TRhs& rhs) const {
      followed_ = true;
      return msg<TShouldError, const TLhs&, const TRhs&, internal::CmpHelperGT>{
          info_, ">", lhs_, rhs};
    }

    template <class TRhs>
    auto operator>=(const TRhs& rhs) const {
      followed_ = true;
      return msg<TShouldError, const TLhs&, const TRhs&, internal::CmpHelperGE>{
          info_, ">=", lhs_, rhs};
    }

    template <class TRhs>
    auto operator<=(const TRhs& rhs) const {
      followed_ = true;
      return msg<TShouldError, const TLhs&, const TRhs&, internal::CmpHelperLE>{
          info_, "<=", lhs_, rhs};
    }

    template <class TRhs>
    auto operator<(const TRhs& rhs) const {
      followed_ = true;
      return msg<TShouldError, const TLhs&, const TRhs&, internal::CmpHelperLT>{
          info_, "<", lhs_, rhs};
    }

    operator bool() const { return result_; }

   private:
    void set_result(const bool& result) {
      result_ = internal::CmpHelperEQ("", "", result, true);
    }

    template <class T>
    std::enable_if_t<std::is_constructible<bool, T>::value> set_result(
        const T& t) {
      set_result(bool(t));
    }

    template <class T>
    std::enable_if_t<!std::is_constructible<bool, T>::value> set_result(
        const T&) {}

    void assert_error(const bool& result) {
      if (TShouldError::value && !followed_) {
        const AssertionResult gtest_ar =
            (internal::CmpHelperEQ(info_.expr.c_str(), "true", result, true));
        if (!gtest_ar) {
          internal::AssertHelper(info_.failure, info_.file, info_.line,
                                 gtest_ar.failure_message()) = *this;
        }
      }
    }

    template <class T>
    std::enable_if_t<std::is_constructible<bool, T>::value> assert_error(
        const T& t) {
      assert_error(bool(t));
    }

    template <class T>
    std::enable_if_t<!std::is_constructible<bool, T>::value> assert_error(
        const T&) {}

    info info_{};
    TLhs lhs_{};
    mutable bool followed_{false};
    bool result_{false};
  };

 public:
  explicit op(const info& info) : info_{info} {}

  template <class TLhs>
  comp<TLhs> operator<<(const TLhs& lhs) const {
    return comp<TLhs>{info_, lhs};
  }

  comp<std::string> operator<<(const char* lhs) const {
    return comp<std::string>{info_, std::string{lhs}};
  }

 private:
  info info_{};
};

struct drop {
  template <class T>
  drop& operator<<(const T&) {
    return *this;
  }
};

struct ret_void {
  template <class T>
  void operator==(const T&) {}
};

template <typename T>
void prevent_commas(T&&) {}

}  // namespace detail
}  // namespace v1
}  // namespace testing

#define GUNIT_PREVENT_COMMAS(...) \
  decltype(testing::detail::prevent_commas(__VA_ARGS__))()

#define EXPECT_IMPL(...)                                                     \
  (::testing::detail::op<std::true_type>{                                    \
       ::testing::detail::info{__FILE__, __LINE__, #__VA_ARGS__,             \
                               ::testing::TestPartResult::kNonFatalFailure}} \
   << __VA_ARGS__)

#define EXPECT(...)                  \
  GUNIT_PREVENT_COMMAS(__VA_ARGS__); \
  EXPECT_IMPL(__VA_ARGS__)

#define ASSERT_IMPL(...)                                                     \
  if (::testing::detail::op<std::false_type>{                                \
          ::testing::detail::info{__FILE__, __LINE__, #__VA_ARGS__,          \
                                  ::testing::TestPartResult::kFatalFailure}} \
      << __VA_ARGS__)                                                        \
    void(::testing::detail::drop{});                                         \
  else                                                                       \
    return ::testing::detail::ret_void{} ==                                  \
           (::testing::detail::op<std::true_type>{::testing::detail::info{   \
                __FILE__, __LINE__, #__VA_ARGS__,                            \
                ::testing::TestPartResult::kFatalFailure}}                   \
            << __VA_ARGS__)

#define ASSERT(...)                  \
  GUNIT_PREVENT_COMMAS(__VA_ARGS__); \
  ASSERT_IMPL(__VA_ARGS__)
