//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <gtest/gtest.h>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include "GMock.h"

#define GTEST_REQUIRES(...) typename std::enable_if<__VA_ARGS__, int>::type = 0

namespace testing {
inline namespace v1 {
namespace detail {

template <typename T>
struct type {
  static void id() {}
};

template <typename T>
std::size_t type_id() {
  return reinterpret_cast<std::size_t>(&type<T>::id);
}

template <class T, class... TArgs>
decltype(void(T{std::declval<TArgs>()...}), std::true_type{}) test_is_braces_constructible(int);
template <class, class...>
std::false_type test_is_braces_constructible(...);
template <class T, class... TArgs>
using is_braces_constructible = decltype(test_is_braces_constructible<T, TArgs...>(0));
template <class T, class... TArgs>
using is_braces_constructible_t = typename is_braces_constructible<T, TArgs...>::type;

template <class TParent>
struct any_type {
  template <class T, GTEST_REQUIRES(std::is_polymorphic<std::decay_t<T>>::value)>
  operator T&() {
    auto mock = std::make_shared<GMock<std::decay_t<T>>>();
    mocks[type_id<std::decay_t<T>>()] = mock;
    return *mock;
  }

  template <class T, GTEST_REQUIRES(!std::is_polymorphic<T>::value)>
  operator T() {
    return {};
  }

  std::unordered_map<std::size_t, std::shared_ptr<void>>& mocks;
};

template <class T>
auto make() {
  std::unordered_map<std::size_t, std::shared_ptr<void>> mocks;
  return std::make_pair(std::make_unique<T>(any_type<T>{mocks}, any_type<T>{mocks}), mocks);
}
}  // detail

template <class T, class... TArgs>
auto make(TArgs&&... args) {
  return detail::make<T>(/*std::make_tuple(std::forward<TArgs>(args)...)*/);
}

struct uninitialized {};

template <class T>
class GTest : public Test {
 public:
  std::unique_ptr<T> sut;
  std::unordered_map<std::size_t, std::shared_ptr<void>> mocks;

  GTest() { std::tie(sut, mocks) = make<T>(); }

  explicit GTest(uninitialized) {}

  // template <class... TArgs>
  // explicit GTest(TArgs...) {
  // std::tie(sut, mocks) = make<T>(args...);
  //}

  template <class TMock>
  GMock<TMock>& mock() {
    return *static_cast<GMock<TMock>*>(mocks[detail::type_id<TMock>()].get());
  }
};

}  // v1
}  // testing
