//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <tuple>
#include <type_traits>
#include "GUnit/Detail/TypeTraits.h"

namespace testing {
inline namespace v1 {
namespace detail {

template <template <class...> class, class>
struct apply;
template <template <class...> class T, template <class...> class U, class... Ts>
struct apply<T, U<Ts...>> {
  using type = T<Ts...>;
};
template <template <class...> class T, class D>
using apply_t = typename apply<T, D>::type;

template <class T>
struct is_valid_impl {
  template <class... Ts>
  auto operator()(Ts &&...) const {
    return is_callable<T(Ts...)>{};
  }
};

template <class T>
auto is_valid(T) {
  return is_valid_impl<T>{};
}

template <class C, class FT, class FF>
struct constexpr_if_impl {
  C c;
  FT ft;
  FF ff;

  template <class T>
  decltype(auto) operator()(T &&t) {
    return call(c(std::forward<T>(t)), std::forward<T>(t));
  }

  template <class T>
  decltype(auto) call(std::true_type, T &&t) {
    return ft(std::forward<T>(t));
  }

  template <class T>
  decltype(auto) call(std::false_type, T &&t) {
    return ff(std::forward<T>(t));
  }
};

template <class C, class FT, class FF>
decltype(auto) constexpr_if(C &&c, FT &&ft, FF &&ff) {
  return constexpr_if_impl<C, FT, FF>{std::forward<C>(c), std::forward<FT>(ft),
                                      std::forward<FF>(ff)};
}

template <class TDst, class TSrc>
inline TDst union_cast(TSrc src) {
  union {
    TSrc src;
    TDst dst;
  } u;
  u.src = src;
  return u.dst;
}

template <class, class>
struct contains;

template <class T, class... TArgs>
struct contains<T, std::tuple<TArgs...>>
    : std::integral_constant<
          bool,
          !std::is_same<
              std::integer_sequence<bool, false,
                                    std::is_same<T, TArgs>::value...>,
              std::integer_sequence<bool, std::is_same<T, TArgs>::value...,
                                    false>>::value> {};

}  // detail
}  // v1
}  // testing
