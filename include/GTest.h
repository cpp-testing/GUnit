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

struct none {};

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

template <class>
struct wrapper;

template <class T>
struct wrapper<GMock<T>> {
  operator std::shared_ptr<T>() { return std::static_pointer_cast<T>(mock); }
  operator T*() { return reinterpret_cast<T*>(mock.get()); }
  operator T&() { return *mock; }
  std::shared_ptr<GMock<T>> mock;
};

template <class TParent, class TArgs = none>
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

  template <class T, GTEST_REQUIRES(!is_copy_ctor<TParent, T>::value && std::is_polymorphic<deref_t<T>>::value)>
  operator T() {
    using mock_t = GMock<deref_t<T>>;
    auto mock = std::make_shared<mock_t>();
    mocks[type_id<deref_t<T>>()] = mock;
    return wrapper<mock_t>{mock};
  }

  template <class T, GTEST_REQUIRES(!is_copy_ctor<TParent, T>::value && std::is_polymorphic<deref_t<T>>::value  && !is_shared_ptr<T>::value)>
  operator T&() const {
    using mock_t = GMock<deref_t<T>>;
    auto mock = std::make_shared<GMock<deref_t<T>>>();
    mocks[type_id<deref_t<T>>()] = mock;
    return wrapper<mock_t>{mock};
  }

  template <class T, GTEST_REQUIRES(!is_copy_ctor<TParent, T>::value && std::is_polymorphic<deref_t<T>>::value && !is_shared_ptr<T>::value)>
  operator const T&() const {
    using mock_t = GMock<deref_t<T>>;
    auto mock = std::make_shared<GMock<deref_t<T>>>();
    mocks[type_id<deref_t<T>>()] = mock;
    return wrapper<mock_t>{mock};
  }

  template <class T, GTEST_REQUIRES(!is_copy_ctor<TParent, T>::value && !std::is_polymorphic<deref_t<T>>::value)>
  operator T() {
    std::size_t elem = {};
    return get<T>(elem, std::integral_constant<bool, contains<T, TArgs>::value>{}, std::tuple_size<TArgs>{},
                  std::integral_constant<std::size_t, 0>{});
  }

  template <class T, GTEST_REQUIRES(!is_copy_ctor<TParent, T>::value && !std::is_polymorphic<deref_t<T>>::value) >
  operator T&() const {
    std::size_t elem = {};
    return const_cast<any_type*>(this)->get<T&>(elem, std::integral_constant<bool, contains<T&, TArgs>::value>{},
                                                std::tuple_size<TArgs>{}, std::integral_constant<std::size_t, 0>{});
  }

  template <class T, GTEST_REQUIRES(!is_copy_ctor<TParent, T>::value && !std::is_polymorphic<deref_t<T>>::value)>
  operator const T&() const {
    std::size_t elem = {};
    return const_cast<any_type*>(this)->get<const T&>(elem, std::integral_constant<bool, contains<const T&, TArgs>::value>{},
                                                      std::tuple_size<TArgs>{}, std::integral_constant<std::size_t, 0>{});
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
struct ctor_size<T, std::index_sequence<Ns...>> : std::conditional_t<std::is_constructible<T, any_type_t<Ns, T>...>::value,
                                                                     std::integral_constant<std::size_t, sizeof...(Ns)>,
                                                                     ctor_size<T, std::make_index_sequence<sizeof...(Ns)-1>>> {
};

template <class T, class... TArgs, std::size_t... Ns>
auto MakeImpl(std::unordered_map<std::size_t, std::shared_ptr<void>>& mocks, std::tuple<TArgs...>& args,
              std::index_sequence<Ns...>) {
  std::unordered_map<std::size_t, std::size_t> arg_nums;
  return std::make_unique<T>(any_type_t<Ns, T, std::tuple<TArgs...>>{mocks, args, arg_nums}...);
}

template <class T, class... TArgs>
auto Make(std::tuple<TArgs...>& args) {
  std::unordered_map<std::size_t, std::shared_ptr<void>> mocks;
  return std::make_pair(MakeImpl<T>(mocks, args, std::make_index_sequence<ctor_size<T>::value>{}), mocks);
}

}  // detail

template <class T>
class GTest : public Test {
 public:
  void SetUp() override final {
    if (!sut.get()) {
      std::tie(sut, mocks) = Make<T>();
    }
  }

  void TearDown() override final { }

  template <class U = T, class... TArgs>
  auto Make(TArgs&&... args) {
    static_assert(std::is_same<T, U>::value, "Make<T> requires the same type as GTest<T>");
    std::tuple<TArgs...> tuple{std::forward<TArgs>(args)...};
    return detail::Make<T>(tuple);
  }

  template <class TMock>
  decltype(auto) Mock() {
    return *static_cast<GMock<TMock>*>(mocks[detail::type_id<TMock>()].get());
  }

 protected:
  std::unique_ptr<T> sut;
  std::unordered_map<std::size_t, std::shared_ptr<void>> mocks;
};

}  // v1
}  // testing
