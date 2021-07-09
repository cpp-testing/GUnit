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

template<class TLhs, class TRhs, AssertionResult (*Comp)(const char*, const char*, TLhs, TRhs)>
class msg : public decltype(Message()) {
 public:
  msg(const info& info, const std::string& comp, TLhs lhs, TRhs rhs) : info_{info}, comp_{comp}, lhs_{lhs}, rhs_{rhs} { }
 ~msg() {
    const auto begin = info_.expr.find(comp_);
    auto lhs_expr = info_.expr.substr(0, begin);
    trim(lhs_expr);
    auto rhs_expr = info_.expr.substr(begin+std::size(comp_));
    trim(rhs_expr);
    const AssertionResult gtest_ar = (Comp(lhs_expr.c_str(), rhs_expr.c_str(), lhs_, rhs_));
    if (!gtest_ar) {
      internal::AssertHelper(info_.failure, info_.file, info_.line, gtest_ar.failure_message()) = *this;
    }
  }

 private:
  info info_{};
  std::string comp_{};
  TLhs lhs_;
  TRhs rhs_;
};

class op {
  template<class TLhs>
  class comp {
   public:
    explicit comp(const info& info, const TLhs& lhs)
      : info_{info}, lhs_{lhs} {}

    template<class TRhs, std::enable_if_t<std::is_floating_point<TLhs>::value || std::is_floating_point<TRhs>::value, int> = 0>
    auto operator==(const TRhs& rhs) const {
      return msg<TLhs, TRhs, internal::CmpHelperFloatingPointEQ<TLhs>>{info_, "==", lhs_, rhs};
    }

    template<class TRhs, std::enable_if_t<!std::is_floating_point<TLhs>::value && !std::is_floating_point<TRhs>::value, int> = 0>
    auto operator==(const TRhs& rhs) const {
      return msg<const TLhs&, const TRhs&, internal::CmpHelperEQ>{info_, "==", lhs_, rhs};
    }

    template<class TRhs>
    auto operator!=(const TRhs& rhs) const {
      return msg<const TLhs&, const TRhs&, internal::CmpHelperNE>{info_, "!=", lhs_, rhs};
    }

    template<class TRhs>
    auto operator>(const TRhs& rhs) const {
      return msg<const TLhs&, const TRhs&, internal::CmpHelperGT>{info_, ">", lhs_, rhs};
    }

    template<class TRhs>
    auto operator>=(const TRhs& rhs) const {
      return msg<const TLhs&, const TRhs&, internal::CmpHelperGE>{info_, ">=", lhs_, rhs};
    }

    template<class TRhs>
    auto operator<=(const TRhs& rhs) const {
      return msg<const TLhs&, const TRhs&, internal::CmpHelperLE>{info_, "<=", lhs_, rhs};
    }

    template<class TRhs>
    auto operator<(const TRhs& rhs) const {
      return msg<const TLhs&, const TRhs&, internal::CmpHelperLT>{info_, "<", lhs_, rhs};
    }

   private:
    info info_{};
    TLhs lhs_{};
  };

 public:
  explicit op(const info& info)
    : info_{info} {}

  template<class TLhs> comp<TLhs> operator%(const TLhs& lhs) const { return comp<TLhs>{info_, lhs}; }

 private:
  info info_{};
};
} // namespace detail
} // namespace v1
} // namespace testing

#define EXPECT(...) (::testing::detail::op{::testing::detail::info{__FILE__,  __LINE__, #__VA_ARGS__, ::testing::TestPartResult::kNonFatalFailure}} % __VA_ARGS__)
#define ASSERT(...) (::testing::detail::op{::testing::detail::info{__FILE__,  __LINE__, #__VA_ARGS__, ::testing::TestPartResult::kFatalFailure}} % __VA_ARGS__)
