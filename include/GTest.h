//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <gtest/gtest.h>
#include <memory>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include "GMock.h"

#define GTEST_REQUIRES(...) typename std::enable_if<__VA_ARGS__, int>::type = 0
#if !defined(GTEST_MAX_CTOR_SIZE)
#define GTEST_MAX_CTOR_SIZE 10
#endif

namespace testing {
inline namespace v1 {
namespace detail {
template <class T>
struct type {
  static void id() {}
};

template <class T>
auto type_id() {
  return reinterpret_cast<std::size_t>(&type<T>::id);
}

template <class, class>
struct contains;

template <class T, class... TArgs>
struct contains<T, std::tuple<TArgs...>>
    : std::integral_constant<bool, !std::is_same<std::integer_sequence<bool, false, std::is_same<T, TArgs>::value...>,
                                                 std::integer_sequence<bool, std::is_same<T, TArgs>::value..., false>>::value> {
};

struct none {};

template <class TParent, class TArgs = none>
struct any_type {
  template <class T, GTEST_REQUIRES(std::is_polymorphic<std::decay_t<T>>::value)>
  operator T() {
    ++arg_nums[type_id<T>()];
    auto mock = std::make_shared<GMock<std::decay_t<T>>>();
    mocks[type_id<std::decay_t<T>>()] = mock;
    return *mock;
  }

  template <class T, GTEST_REQUIRES(std::is_polymorphic<std::decay_t<T>>::value)>
  operator T&() const {
    ++arg_nums[type_id<T&>()];
    auto mock = std::make_shared<GMock<std::decay_t<T>>>();
    mocks[type_id<std::decay_t<T>>()] = mock;
    return *mock;
  }

  template <class T, GTEST_REQUIRES(std::is_polymorphic<std::decay_t<T>>::value)>
  operator const T&() const {
    ++arg_nums[type_id<const T&>()];
    auto mock = std::make_shared<GMock<std::decay_t<T>>>();
    mocks[type_id<std::decay_t<T>>()] = mock;
    return *mock;
  }

  template <class T, std::size_t N>
  T get(std::true_type, std::integral_constant<std::size_t, N>, std::integral_constant<std::size_t, N>) {
    // static_assert(std::is_same<T, void>::value, "dypa");
    static std::decay_t<T> type;  // default
    return type;
  }

  template <class T, std::size_t Max, std::size_t N>
  T get(std::true_type v, std::integral_constant<std::size_t, Max> max, std::integral_constant<std::size_t, N>) {
    if (N == arg_nums[type_id<T>()]) {
      ++arg_nums[type_id<T>()];
      std::cout << "GET: " << N << " " << std::get<N>(args) << std::endl;
      return std::get<N>(args);
    }
    return get<T>(v, max, std::integral_constant<std::size_t, N + 1>{});
  }

  template <class T>
  T get(std::false_type, ...) {
    ++arg_nums[type_id<T>()];
    return {};
  }

  template <class T, GTEST_REQUIRES(!std::is_polymorphic<std::decay_t<T>>::value)>
  operator T() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    return get<T>(std::integral_constant<bool, contains<T, TArgs>::value>{}, std::tuple_size<TArgs>{},
                  std::integral_constant<std::size_t, 0>{});
  }

  template <class T, GTEST_REQUIRES(!std::is_polymorphic<std::decay_t<T>>::value)>
  operator T&() const {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    return const_cast<any_type*>(this)->get<T&>(std::integral_constant<bool, contains<T&, TArgs>::value>{},
                                                std::tuple_size<TArgs>{}, std::integral_constant<std::size_t, 0>{});
  }

  template <class T, GTEST_REQUIRES(!std::is_polymorphic<std::decay_t<T>>::value)>
  operator const T&() const {
    return get<const T&>(std::integral_constant<bool, contains<const T&, TArgs>::value>{}, std::tuple_size<TArgs>{},
                         std::integral_constant<std::size_t, 0>{});
  }

  std::unordered_map<std::size_t, std::shared_ptr<void>>& mocks;
  TArgs& args;
  std::unordered_map<std::size_t, std::size_t>& arg_nums;
};

template <std::size_t, class T, class TArgs = none>
using any_type_t = any_type<T, TArgs>;

template <class, class = std::make_index_sequence<GTEST_MAX_CTOR_SIZE>>
struct ctor_size;

template <class T>
struct ctor_size<T, std::index_sequence<>> : std::integral_constant<std::size_t, 0> {};

template <class T, std::size_t... Ns>
struct ctor_size<T, std::index_sequence<Ns...>>
    : std::conditional_t<std::is_constructible<T, any_type_t<Ns, T>...>::value,
                         std::integral_constant<std::size_t, sizeof...(Ns)>,
                         ctor_size<T, std::make_index_sequence<sizeof...(Ns) - 1>>> {};

template <class T, class... TArgs, std::size_t... Ns>
auto create(std::unordered_map<std::size_t, std::shared_ptr<void>>& mocks, std::tuple<TArgs...>& args,
            std::index_sequence<Ns...>) {
  std::unordered_map<std::size_t, std::size_t> arg_nums;
  return std::make_unique<T>(any_type_t<Ns, T, std::tuple<TArgs...>>{mocks, args, arg_nums}...);
}

template <class T, class... TArgs>
auto make(std::tuple<TArgs...>& args) {
  std::unordered_map<std::size_t, std::shared_ptr<void>> mocks;
  return std::make_pair(create<T>(mocks, args, std::make_index_sequence<ctor_size<T>::value>{}), mocks);
}

template <class T>
class GTestImpl : public Test {
 public:
  template <class U, class... TArgs>
  void make_(TArgs&&... args) {
    static_assert(std::is_same<T, U>::value, "");
    std::tuple<TArgs...> tuple{std::forward<TArgs>(args)...};
    std::tie(sut, mocks) = make<T>(tuple);
  }

  template <class TMock>
  GMock<TMock>& mock() {
    return *static_cast<GMock<TMock>*>(mocks[type_id<TMock>()].get());
  }

 protected:
  std::unique_ptr<T> sut;
  std::unordered_map<std::size_t, std::shared_ptr<void>> mocks;
};

}  // detail

template <class T, class... TArgs>
auto make(TArgs&&... args) {
  using swallow = int[];
  (void)swallow{0, (std::cout << args << std::endl, 0)...};
  std::tuple<TArgs...> tuple{std::forward<TArgs>(args)...};
  std::cout << "DUA: " << std::get<1>(tuple) << std::endl;
  return detail::make<T>(tuple);
}

using uninitialized = std::false_type;

template <class T, class TInitialized = std::true_type>
class GTest : public detail::GTestImpl<T> {
 public:
  GTest() { std::tie(detail::GTestImpl<T>::sut, detail::GTestImpl<T>::mocks) = make<T>(); }
};

template <class T>
class GTest<T, uninitialized> : public detail::GTestImpl<T> {
 public:
  GTest() = default;
};

}  // v1
}  // testing
