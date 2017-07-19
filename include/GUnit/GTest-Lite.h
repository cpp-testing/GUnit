//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

namespace detail {

template <bool, char...>
struct test;

template <char... Chars>
struct test<true, Chars...> {
  template <class Test>
  bool operator=(const Test& test) {
    test();
    return true;
  }
};

template <char... Chars>
struct test<false, Chars...> {
  template <class Test>
  bool operator=(const Test&) {
    return false;
  }
};

}  // detail

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif

template <class T, T... Chars>
constexpr auto operator""_test() {
  return detail::test<true, Chars...>{};
}

template <class T, T... Chars>
constexpr auto operator""_test_disabled() {
  return detail::test<false, Chars...>{};
}
