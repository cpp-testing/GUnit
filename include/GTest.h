//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
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
  return reinterpret_cast<std::size_t>(&type<std::remove_cv_t<T>>::id);
}

template <class T>
struct deref {
  using type = std::remove_cv_t<std::remove_reference_t<std::remove_pointer_t<T>>>;
};

template <class T, class TDeleter>
struct deref<std::unique_ptr<T, TDeleter>> {
  using type = typename deref<T>::type;
};

template <class T>
struct deref<std::shared_ptr<T>> {
  using type = typename deref<T>::type;
};

template <class T>
struct deref<std::weak_ptr<T>> {
  using type = typename deref<T>::type;
};

template <class T>
using deref_t = typename deref<std::remove_cv_t<T>>::type;

template <class>
struct is_shared_ptr_impl : std::false_type {};

template <class T>
struct is_shared_ptr_impl<std::shared_ptr<T>> : std::true_type {};

template <class T>
using is_shared_ptr = typename is_shared_ptr_impl<std::remove_cv_t<T>>::type;

template <class T, class U>
using is_copy_ctor = std::is_same<deref_t<T>, deref_t<U>>;

template <class, class>
struct contains;

template <class T, class... TArgs>
struct contains<T, std::tuple<TArgs...>>
    : std::integral_constant<bool, !std::is_same<std::integer_sequence<bool, false, std::is_same<T, TArgs>::value...>,
                                                 std::integer_sequence<bool, std::is_same<T, TArgs>::value..., false>>::value> {
};

template <class T>
struct wrapper {
  operator std::shared_ptr<T>() { return std::static_pointer_cast<T>(mock); }
  operator std::unique_ptr<T>() {
    // mock.reset(mock.get(), [](void*){ });
    return std::unique_ptr<T>(reinterpret_cast<T*>(mock.get()));
  }
  operator T*() { return reinterpret_cast<T*>(mock.get()); }
  operator T&() { return *reinterpret_cast<T*>(mock.get()); }
  std::shared_ptr<void>& mock;
};

}  // detail

class mocks_t : public std::unordered_map<std::size_t, std::shared_ptr<void>> {
 public:
  template <class TMock>
  decltype(auto) mock() {
    return *static_cast<GMock<TMock>*>((*this)[detail::type_id<TMock>()].get());
  }
};

namespace detail {

template <class TParent, class TArgs = std::tuple<>>
struct any_type {
  template <class T, std::size_t N>
  T get(std::size_t&, std::true_type, std::integral_constant<std::size_t, N>, std::integral_constant<std::size_t, N>) {
    static std::decay_t<T> type;  // default
    return type;
  }

  template <class T, std::size_t Max, std::size_t N>
  T get(std::size_t& elem, std::true_type v, std::integral_constant<std::size_t, Max> max,
        std::integral_constant<std::size_t, N>) {
    if (type_id<T>() == type_id<std::tuple_element_t<N, TArgs>>()) ++elem;
    if (elem - 1 == arg_nums[type_id<T>()]) {
      ++arg_nums[type_id<T>()];
      return (T)std::get<N>(args);
    }
    return get<T>(elem, v, max, std::integral_constant<std::size_t, N + 1>{});
  }

  template <class T>
  T get(std::size_t&, std::false_type, ...) {
    static std::decay_t<T> type;  // default
    return type;
  }

  template <class T, GREQUIRES(!is_copy_ctor<TParent, T>::value && std::is_polymorphic<deref_t<T>>::value)>
  operator T() {
    mocks[type_id<deref_t<T>>()] = std::make_shared<GMock<deref_t<T>>>();
    return wrapper<deref_t<T>>{mocks[type_id<deref_t<T>>()]};
  }

  template <class T,
            GREQUIRES(!is_copy_ctor<TParent, T>::value && std::is_polymorphic<deref_t<T>>::value && !is_shared_ptr<T>::value)>
  operator T&() const {
    mocks[type_id<deref_t<T>>()] = std::make_shared<GMock<deref_t<T>>>();
    return wrapper<deref_t<T>>{mocks[type_id<deref_t<T>>()]};
  }

  template <class T,
            GREQUIRES(!is_copy_ctor<TParent, T>::value && std::is_polymorphic<deref_t<T>>::value && !is_shared_ptr<T>::value)>
  operator const T&() const {
    mocks[type_id<deref_t<T>>()] = std::make_shared<GMock<deref_t<T>>>();
    return wrapper<deref_t<T>>{mocks[type_id<deref_t<T>>()]};
  }

  template <class T, GREQUIRES(!is_copy_ctor<TParent, T>::value && !std::is_polymorphic<deref_t<T>>::value)>
  operator T() {
    std::size_t elem = {};
    return get<T>(elem, std::integral_constant<bool, contains<T, TArgs>::value>{}, std::tuple_size<TArgs>{},
                  std::integral_constant<std::size_t, 0>{});
  }

  template <class T, GREQUIRES(!is_copy_ctor<TParent, T>::value && !std::is_polymorphic<deref_t<T>>::value)>
  operator T&() const {
    std::size_t elem = {};
    return const_cast<any_type*>(this)->get<T&>(elem, std::integral_constant<bool, contains<T&, TArgs>::value>{},
                                                std::tuple_size<TArgs>{}, std::integral_constant<std::size_t, 0>{});
  }

  template <class T, GREQUIRES(!is_copy_ctor<TParent, T>::value && !std::is_polymorphic<deref_t<T>>::value)>
  operator const T&() const {
    std::size_t elem = {};
    return const_cast<any_type*>(this)->get<const T&>(elem, std::integral_constant<bool, contains<const T&, TArgs>::value>{},
                                                      std::tuple_size<TArgs>{}, std::integral_constant<std::size_t, 0>{});
  }

  mocks_t& mocks;
  TArgs& args;
  std::unordered_map<std::size_t, std::size_t>& arg_nums;
};

template <std::size_t, class T, class TArgs = std::tuple<>>
using any_type_t = any_type<T, TArgs>;

template <class, class = std::make_index_sequence<GTEST_MAX_CTOR_SIZE>>
struct ctor_size;

template <class T>
struct ctor_size<T, std::index_sequence<>> : std::integral_constant<std::size_t, 0> {};

template <class T, std::size_t... Ns>
struct ctor_size<T, std::index_sequence<Ns...>> : std::conditional_t<std::is_constructible<T, any_type_t<Ns, T>...>::value,
                                                                     std::integral_constant<std::size_t, sizeof...(Ns)>,
                                                                     ctor_size<T, std::make_index_sequence<sizeof...(Ns)-1>>> {
};

template <class T, class... TArgs, std::size_t... Ns>
auto make_impl(detail::identity<std::unique_ptr<T>>, mocks_t& mocks, std::tuple<TArgs...>& args, std::index_sequence<Ns...>) {
  std::unordered_map<std::size_t, std::size_t> arg_nums;
  return std::make_unique<T>(any_type_t<Ns, detail::deref_t<T>, std::tuple<TArgs...>>{mocks, args, arg_nums}...);
}

template <class T, class... TArgs, std::size_t... Ns>
auto make_impl(detail::identity<std::shared_ptr<T>>, mocks_t& mocks, std::tuple<TArgs...>& args, std::index_sequence<Ns...>) {
  std::unordered_map<std::size_t, std::size_t> arg_nums;
  return std::make_shared<T>(any_type_t<Ns, detail::deref_t<T>, std::tuple<TArgs...>>{mocks, args, arg_nums}...);
}

template <class T, class... TArgs, std::size_t... Ns>
auto make_impl(detail::identity<T>, mocks_t& mocks, std::tuple<TArgs...>& args, std::index_sequence<Ns...>) {
  std::unordered_map<std::size_t, std::size_t> arg_nums;
  return T(any_type_t<Ns, detail::deref_t<T>, std::tuple<TArgs...>>{mocks, args, arg_nums}...);
}

}  // detail

template <class T, template <class> class TMock = NoMock, GREQUIRES(detail::is_gmock<TMock>::value), class... TArgs>
auto make(TArgs&&... args) {
  std::tuple<TArgs...> tuple{std::forward<TArgs>(args)...};
  mocks_t mocks;
  return std::make_pair(detail::make_impl(detail::identity<T>{}, mocks, tuple,
                                          std::make_index_sequence<detail::ctor_size<detail::deref_t<T>>::value>{}),
                        mocks);
}

template <class T>
class GTest : public Test {
 protected:
  using SUT = std::unique_ptr<T>;

  template <class TMock>
  decltype(auto) mock() {
    return mocks.mock<TMock>();
  }

  SUT sut;
  mocks_t mocks;
};

}  // v1
}  // testing
