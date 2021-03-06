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

template <class TLhs, class TRhs,
          AssertionResult (*Comp)(const char*, const char*, TLhs, TRhs)>
class msg : public decltype(Message()) {
 public:
  msg(const info& info, const std::string& comp, TLhs lhs, TRhs rhs)
      : info_{info}, comp_{comp}, lhs_{lhs}, rhs_{rhs} {}
  ~msg() {
    const auto begin = info_.expr.find(comp_);
    auto lhs_expr = info_.expr.substr(0, begin);
    trim(lhs_expr);
    auto rhs_expr = info_.expr.substr(begin + std::size(comp_));
    trim(rhs_expr);
    const AssertionResult gtest_ar =
        (Comp(lhs_expr.c_str(), rhs_expr.c_str(), lhs_, rhs_));
    if (!gtest_ar) {
      internal::AssertHelper(info_.failure, info_.file, info_.line,
                             gtest_ar.failure_message()) = *this;
    }
  }

 private:
  info info_{};
  std::string comp_{};
  TLhs lhs_;
  TRhs rhs_;
};

class op {
  template <class TLhs>
  class comp : public decltype(Message()) {
   public:
    explicit comp(const info& info, const TLhs& lhs) : info_{info}, lhs_{lhs} {}
    ~comp() {
      if (!followed_ && std::is_same<bool, TLhs>::value) {
        const AssertionResult gtest_ar =
            (internal::CmpHelperEQ(info_.expr.c_str(), "true", lhs_, true));
        if (!gtest_ar) {
          internal::AssertHelper(info_.failure, info_.file, info_.line,
                                 gtest_ar.failure_message()) = *this;
        }
      }
    }

    template <class TRhs,
              std::enable_if_t<std::is_floating_point<TLhs>::value ||
                                   std::is_floating_point<TRhs>::value,
                               int> = 0>
    auto operator==(const TRhs& rhs) const {
      followed_ = true;
      return msg<TLhs, TRhs, internal::CmpHelperFloatingPointEQ<TLhs>>{
          info_, "==", lhs_, rhs};
    }

    template <class TRhs,
              std::enable_if_t<!std::is_floating_point<TLhs>::value &&
                                   !std::is_floating_point<TRhs>::value,
                               int> = 0>
    auto operator==(const TRhs& rhs) const {
      followed_ = true;
      return msg<const TLhs&, const TRhs&, internal::CmpHelperEQ>{
          info_, "==", lhs_, rhs};
    }

    template <class TRhs>
    auto operator!=(const TRhs& rhs) const {
      followed_ = true;
      return msg<const TLhs&, const TRhs&, internal::CmpHelperNE>{
          info_, "!=", lhs_, rhs};
    }

    template <class TRhs>
    auto operator>(const TRhs& rhs) const {
      followed_ = true;
      return msg<const TLhs&, const TRhs&, internal::CmpHelperGT>{info_, ">",
                                                                  lhs_, rhs};
    }

    template <class TRhs>
    auto operator>=(const TRhs& rhs) const {
      followed_ = true;
      return msg<const TLhs&, const TRhs&, internal::CmpHelperGE>{
          info_, ">=", lhs_, rhs};
    }

    template <class TRhs>
    auto operator<=(const TRhs& rhs) const {
      followed_ = true;
      return msg<const TLhs&, const TRhs&, internal::CmpHelperLE>{
          info_, "<=", lhs_, rhs};
    }

    template <class TRhs>
    auto operator<(const TRhs& rhs) const {
      followed_ = true;
      return msg<const TLhs&, const TRhs&, internal::CmpHelperLT>{info_, "<",
                                                                  lhs_, rhs};
    }

   private:
    info info_{};
    TLhs lhs_{};
    mutable bool followed_{false};
  };

 public:
  explicit op(const info& info) : info_{info} {}

  template <class TLhs>
  comp<TLhs> operator%(const TLhs& lhs) const {
    return comp<TLhs>{info_, lhs};
  }

 private:
  info info_{};
};
}  // namespace detail
}  // namespace v1
}  // namespace testing

#define EXPECT(...)                                                            \
  (::testing::detail::op{                                                      \
       ::testing::detail::info{__FILE__, __LINE__, #__VA_ARGS__,               \
                               ::testing::TestPartResult::kNonFatalFailure}} % \
   __VA_ARGS__)
#define ASSERT(...)                                                         \
  (::testing::detail::op{                                                   \
       ::testing::detail::info{__FILE__, __LINE__, #__VA_ARGS__,            \
                               ::testing::TestPartResult::kFatalFailure}} % \
   __VA_ARGS__)
