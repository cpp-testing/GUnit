//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <memory>
#include <stdexcept>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include "GUnit/Detail/StringUtils.h"
#include "GUnit/Detail/TypeTraits.h"
#include "GUnit/Detail/Utility.h"
#include "GUnit/GMock.h"

#if !defined(GUNIT_MAX_CTOR_SIZE)
#define GUNIT_MAX_CTOR_SIZE 10
#endif

namespace testing {
inline namespace v1 {
namespace detail {

template <class T>
struct deref {
  using type =
      std::remove_cv_t<std::remove_reference_t<std::remove_pointer_t<T>>>;
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
struct deref<NaggyMock<T>> {
  using type = typename deref<T>::type;
};

template <class T>
struct deref<StrictGMock<T>> {
  using type = typename deref<T>::type;
};

template <class T>
struct deref<NiceGMock<T>> {
  using type = typename deref<T>::type;
};

template <class T>
using deref_t = typename deref<std::remove_cv_t<T>>::type;

template <template <class> class>
struct is_gmock : std::false_type {};

template <>
struct is_gmock<NaggyGMock> : std::true_type {};

template <>
struct is_gmock<StrictGMock> : std::true_type {};

template <>
struct is_gmock<NiceGMock> : std::true_type {};

template <class>
struct is_gmock_type : std::false_type {};

template <class T>
struct is_gmock_type<NaggyGMock<T>> : std::true_type {};

template <class T>
struct is_gmock_type<StrictGMock<T>> : std::true_type {};

template <class T>
struct is_gmock_type<NiceGMock<T>> : std::true_type {};

template <class T, class U>
using is_copy_ctor = std::is_same<deref_t<T>, deref_t<U>>;

template <class T>
struct wrapper {
  operator T &() { return *reinterpret_cast<T *>(mock.get()); }
  operator T *() { return reinterpret_cast<T *>(mock.get()); }
  operator std::unique_ptr<T>() {
    return std::unique_ptr<T>(reinterpret_cast<T *>(mock.get()));
  }
  operator std::shared_ptr<T>() { return std::static_pointer_cast<T>(mock); }
  std::shared_ptr<void> &mock;
};

template <class T>
decltype(auto) convert(GMock<T> *mock) {
  return &static_cast<T &>(*mock);
}

template <class T>
decltype(auto) convert(StrictGMock<T> *mock) {
  return &static_cast<T &>(*mock);
}

template <class T>
decltype(auto) convert(NiceGMock<T> *mock) {
  return &static_cast<T &>(*mock);
}

template <class T>
decltype(auto) convert(GMock<T> &mock) {
  return static_cast<T &>(mock);
}

template <class T>
decltype(auto) convert(StrictGMock<T> &mock) {
  return static_cast<T &>(mock);
}

template <class T>
decltype(auto) convert(NiceGMock<T> &mock) {
  return static_cast<T &>(mock);
}

template <class T>
decltype(auto) convert(T &&arg) {
  return std::forward<T>(arg);
}

template <class T>
decltype(auto) convert(std::unique_ptr<GMock<T>> &&mock) {
  return std::move(mock);
}

template <class T>
decltype(auto) convert(std::unique_ptr<StrictGMock<T>> &&mock) {
  return std::move(mock);
}

template <class T>
decltype(auto) convert(std::unique_ptr<NiceGMock<T>> &&mock) {
  return std::move(mock);
}

template <class T>
decltype(auto) convert(std::shared_ptr<GMock<T>> &mock) {
  return std::static_pointer_cast<T>(mock);
}

template <class T>
decltype(auto) convert(std::shared_ptr<StrictGMock<T>> &mock) {
  return std::static_pointer_cast<T>(mock);
}

template <class T>
decltype(auto) convert(std::shared_ptr<NiceGMock<T>> &mock) {
  return std::static_pointer_cast<T>(mock);
}

template <class T, class... TArgs>
auto make_impl(detail::identity<std::unique_ptr<T>>, TArgs &&... args) {
  return std::make_unique<T>(detail::convert(std::forward<TArgs>(args))...);
}

template <class T, class... TArgs>
auto make_impl(detail::identity<std::shared_ptr<T>>, TArgs &&... args) {
  return std::make_shared<T>(detail::convert(std::forward<TArgs>(args))...);
}

template <class T, class... TArgs>
auto make_impl(detail::identity<T>, TArgs &&... args) {
  return T(detail::convert(std::forward<TArgs>(args))...);
}
}  // detail

template <class>
class mock_exception final : public std::exception {
 public:
  explicit mock_exception(const std::string &msg) : msg(msg) {}
  const char *what() const throw() override { return msg.c_str(); }

 private:
  std::string msg;
};

class mocks_t : public std::unordered_map<std::size_t, std::shared_ptr<void>> {
 public:
  template <class TMock>
  decltype(auto) mock() const {
    const auto it = find(detail::type_id<TMock>());
    if (it == end()) {
      throw mock_exception<TMock>{std::string{"Requested mock \""} +
                                  typeid(TMock).name() + "\" wasn't created!"};
    }
    return *static_cast<GMock<TMock> *>(it->second.get());
  }

  template <class TMock>
  void add() {
    if (find(detail::type_id<TMock>()) != end()) {
      throw mock_exception<TMock>{std::string{"Requested mock \""} +
                                  typeid(TMock).name() +
                                  "\" was already created!"};
    }
    emplace(detail::type_id<typename TMock::type>(), std::make_shared<TMock>());
  }

  template <class TMock>
  auto get() const {
    const auto it = find(detail::type_id<TMock>());
    if (it == end()) {
      throw mock_exception<TMock>{std::string{"Requested mock \""} +
                                  typeid(TMock).name() + "\" wasn't created!"};
    }
    return std::static_pointer_cast<TMock>(it->second);
  }
};

namespace detail {
template <class>
struct required_type_not_found {};
template <class>
struct required_type_is_ambigious {};

template <class TParent>
struct resolve_size {
  template <class T, GUNIT_REQUIRES(!is_copy_ctor<TParent, T>::value)>
  operator T();

#if defined(__GNUC__)
  template <class T, GUNIT_REQUIRES(!is_copy_ctor<TParent, T>::value)>
  operator T &&() const;
#endif

  template <class T, GUNIT_REQUIRES(!is_copy_ctor<TParent, T>::value)>
  operator T &() const;

  template <class T, GUNIT_REQUIRES(!is_copy_ctor<TParent, T>::value)>
  operator const T &() const;
};

template <std::size_t, class T>
using resolve_size_t = resolve_size<T>;

template <class TParent>
struct resolve_creatable {
  template <class T, GUNIT_REQUIRES(!is_copy_ctor<TParent, T>::value &&
                                    std::is_polymorphic<deref_t<T>>::value)>
  operator T();

#if defined(__GNUC__)
  template <class T, GUNIT_REQUIRES(!is_copy_ctor<TParent, T>::value &&
                                    std::is_polymorphic<deref_t<T>>::value)>
  operator T &&() const;
#endif

  template <class T, GUNIT_REQUIRES(!is_copy_ctor<TParent, T>::value &&
                                    std::is_polymorphic<deref_t<T>>::value)>
  operator T &() const;

  template <class T, GUNIT_REQUIRES(!is_copy_ctor<TParent, T>::value &&
                                    std::is_polymorphic<deref_t<T>>::value)>
  operator const T &() const;
};

template <std::size_t, class T>
using resolve_creatable_t = resolve_creatable<T>;

template <class TParent, template <class> class TMock,
          class TArgs = std::tuple<>>
class resolve {
 public:
  resolve(mocks_t &mocks, TArgs &args) : mocks(mocks), args(args) {}

  template <class T, GUNIT_REQUIRES(!is_copy_ctor<TParent, T>::value &&
                                    std::is_polymorphic<deref_t<T>>::value &&
                                    !contains<T, TArgs>::value)>
  operator T() {
    return mock<T>();
  }

  template <class T, GUNIT_REQUIRES(!is_copy_ctor<TParent, T>::value &&
                                    std::is_polymorphic<deref_t<T>>::value &&
                                    !is_shared_ptr<T>::value &&
                                    !contains<T &, TArgs>::value)>
  operator T &() const {
    return mock<T>();
  }

  template <class T, GUNIT_REQUIRES(!is_copy_ctor<TParent, T>::value &&
                                    std::is_polymorphic<deref_t<T>>::value &&
                                    !is_shared_ptr<T>::value &&
                                    contains<const T &, TArgs>::value)>
  operator const T &() const {
    return mock<T>();
  }

  template <class T, GUNIT_REQUIRES(!is_copy_ctor<TParent, T>::value &&
                                    (contains<T, TArgs>::value ||
                                     contains<T &, TArgs>::value ||
                                     contains<const T &, TArgs>::value))>
  operator T() {
    return get(detail::type<T>{}, typename contains<T, TArgs>::type{},
               typename contains<T &, TArgs>::type{},
               typename contains<const T &, TArgs>::type{});
  }

  template <class T, GUNIT_REQUIRES(!is_copy_ctor<TParent, T>::value &&
                                    contains<T &, TArgs>::value)>
  operator T &() const {
    return const_cast<resolve *>(this)->get(detail::type<T &>{});
  }

  template <class T, GUNIT_REQUIRES(!is_copy_ctor<TParent, T>::value &&
                                    contains<const T &, TArgs>::value)>
  operator const T &() const {
    return const_cast<resolve *>(this)->get(detail::type<const T &>{});
  }

 private:
  template <class T>
  decltype(auto) get(detail::type<T>, std::true_type = {}, ...) {
    return std::get<T>(args);
  }

  template <class T>
  decltype(auto) get(detail::type<T>, std::false_type, std::false_type,
                     std::false_type) {
    return required_type_not_found<T>();
  }

  template <class T>
  decltype(auto) get(detail::type<T>, std::false_type, std::true_type,
                     std::false_type) {
    return std::get<T &>(args);
  }

  template <class T>
  decltype(auto) get(detail::type<T>, std::false_type, std::false_type,
                     std::true_type) {
    return std::get<const T &>(args);
  }

  template <class T>
  decltype(auto) get(detail::type<T>, std::false_type, std::true_type,
                     std::true_type) {
    return required_type_is_ambigious<T>();
  }

  template <class T>
  decltype(auto) mock() const {
    const auto id = type_id<deref_t<T>>();
    const auto it = mocks.find(id);
    if (it != mocks.end()) {
      return wrapper<deref_t<T>>{it->second};
    }
    mocks.emplace(id, std::make_shared<TMock<deref_t<T>>>());
    return wrapper<deref_t<T>>{mocks[id]};
  }

  mocks_t &mocks;
  TArgs &args;
};

template <std::size_t, class T, template <class> class TMock,
          class TArgs = std::tuple<>>
using resolve_t = resolve<T, TMock, TArgs>;

template <class, class = std::make_index_sequence<GUNIT_MAX_CTOR_SIZE>>
struct ctor_size;

template <class T>
struct ctor_size<T, std::index_sequence<>>
    : std::integral_constant<std::size_t, 0> {};

template <class T, std::size_t... Ns>
struct ctor_size<T, std::index_sequence<Ns...>>
    : std::conditional_t<
          std::is_constructible<T, resolve_size_t<Ns, T>...>::value,
          std::integral_constant<std::size_t, sizeof...(Ns)>,
          ctor_size<T, std::make_index_sequence<sizeof...(Ns) - 1>>> {};

template <template <class> class TMock, class T, class... TArgs,
          std::size_t... Ns>
auto make_impl(detail::identity<std::unique_ptr<T>>, mocks_t &mocks,
               std::tuple<TArgs...> &args, std::index_sequence<Ns...>) {
  return std::make_unique<T>(
      resolve_t<Ns, detail::deref_t<T>, TMock, std::tuple<TArgs...>>{mocks,
                                                                     args}...);
}

template <template <class> class TMock, class T, class... TArgs,
          std::size_t... Ns>
auto make_impl(detail::identity<std::shared_ptr<T>>, mocks_t &mocks,
               std::tuple<TArgs...> &args, std::index_sequence<Ns...>) {
  return std::make_shared<T>(
      resolve_t<Ns, detail::deref_t<T>, TMock, std::tuple<TArgs...>>{mocks,
                                                                     args}...);
}

template <template <class> class TMock, class T, class... TArgs,
          std::size_t... Ns>
auto make_impl(detail::identity<T>, mocks_t &mocks, std::tuple<TArgs...> &args,
               std::index_sequence<Ns...>) {
  return T(resolve_t<Ns, detail::deref_t<T>, TMock, std::tuple<TArgs...>>{
      mocks, args}...);
}

template <class, class>
struct is_creatable_impl;

template <class T, std::size_t... Ns>
struct is_creatable_impl<T, std::index_sequence<Ns...>>
    : std::is_constructible<T, resolve_creatable_t<Ns, T>...> {};

template <class T>
using is_creatable =
    is_creatable_impl<T, std::make_index_sequence<ctor_size<T>::value>>;

}  // detail

template <class T, class... TArgs>
auto make(TArgs &&... args) {
  return detail::make_impl(detail::identity<T>{}, std::forward<TArgs>(args)...);
}

template <
    class T, template <class> class TMock, class... TMocks,
    GUNIT_REQUIRES(
        detail::is_gmock<TMock>::value &&std::is_same<
            detail::bool_list<detail::always<TMocks>::value...>,
            detail::bool_list<detail::is_gmock_type<TMocks>::value...>>::value),
    class... TArgs>
auto make(TArgs &&... args) {
  std::tuple<TArgs...> tuple{std::forward<TArgs>(args)...};
  mocks_t mocks;
  using swallow = int[];
  (void)swallow{0, (mocks.emplace(detail::type_id<detail::deref_t<TMocks>>(),
                                  std::make_shared<TMocks>()),
                    0)...};
  return std::make_pair(
      detail::make_impl<TMock>(
          detail::identity<T>{}, mocks, tuple,
          std::make_index_sequence<
              detail::ctor_size<detail::deref_t<T>>::value>{}),
      mocks);
}
}  // v1
}  // testing

// clang-format off
#if __has_include(<boost/di.hpp>)
// clang-format on
#include <boost/di.hpp>

BOOST_DI_NAMESPACE_BEGIN

namespace detail {
template <template <class> class TMock>
class Mock {
 public:
  explicit Mock(testing::mocks_t &mocks) : mocks(mocks) {}

  template <class TInjector, class TDependency>
  auto operator()(const TInjector &, const TDependency &) {
    mocks.add<TMock<typename TDependency::expected>>();
    return mocks.get<typename TDependency::expected>();
  }

 private:
  testing::mocks_t &mocks;
};
}  // detail

using GMock = detail::Mock<testing::GMock>;
using NaggyGMock = detail::Mock<testing::NaggyGMock>;
using StrictGMock = detail::Mock<testing::StrictGMock>;
using NiceGMock = detail::Mock<testing::NiceGMock>;

BOOST_DI_NAMESPACE_END

namespace testing {
inline namespace v1 {
template <class T, class TInjector,
          GUNIT_REQUIRES(std::is_base_of<boost::di::core::injector_base,
                                         TInjector>::value)>
decltype(auto) make(const TInjector &injector) {
  return injector.template create<T>();
}
}  // v1
}  // testing
#endif
