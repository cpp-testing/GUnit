//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <gmock/gmock.h>
#include <fstream>
#include <functional>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <tuple>
#include <typeinfo>
#include <unordered_map>
#include <vector>
#include "GUnit/Detail/FileUtils.h"
#include "GUnit/Detail/Preprocessor.h"
#include "GUnit/Detail/ProgUtils.h"
#include "GUnit/Detail/StringUtils.h"
#include "GUnit/Detail/TypeTraits.h"
#include "GUnit/Detail/Utility.h"

#if defined(__clang__)
#pragma clang optimize off
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#elif defined(__GNUC__)
#pragma GCC system_header
#pragma GCC push_options
#pragma GCC optimize("O0")
#endif

namespace testing {
namespace internal {
template <class R, class... TArgs>
class FunctionMocker<R(TArgs...)>
    : public internal::FunctionMockerBase<R(TArgs...)> {
 public:
  using F = R(TArgs...);
  using ArgumentTuple = typename internal::Function<F>::ArgumentTuple;

  MockSpec<F> &With(const Matcher<TArgs> &... args) {
    this->current_spec().SetMatchers(std::make_tuple(args...));
    return this->current_spec();
  }

  R Invoke(TArgs... args) { return this->InvokeWith(ArgumentTuple(args...)); }
};

template <class... TArgs>
struct MatcherTuple<std::tuple<TArgs...>> {
  using type = std::tuple<Matcher<TArgs>...>;
};

template <class R, class... TArgs>
struct Function<R(TArgs...)> {
  using Result = R;
  using ArgumentTuple = std::tuple<TArgs...>;
  using ArgumentMatcherTuple = typename MatcherTuple<ArgumentTuple>::type;
  using MakeResultVoid = void(TArgs...);
  using MakeResultIgnoredValue = IgnoredValue(TArgs...);
};
}  // internal

inline namespace v1 {
namespace detail {

// clang-format off
/**
 * Credits goes to the authors of HippoMocks/FakeIt
 */
struct virtual_offset {
  std::size_t offset;
  virtual std::size_t offset0(int) { return offset = 0; }     virtual std::size_t offset1(int) { return offset = 1; }     virtual std::size_t offset2(int) { return offset = 2; }
  virtual std::size_t offset3(int) { return offset = 3; }     virtual std::size_t offset4(int) { return offset = 4; }     virtual std::size_t offset5(int) { return offset = 5; }
  virtual std::size_t offset6(int) { return offset = 6; }     virtual std::size_t offset7(int) { return offset = 7; }     virtual std::size_t offset8(int) { return offset = 8; }
  virtual std::size_t offset9(int) { return offset = 9; }     virtual std::size_t offset10(int) { return offset = 10; }   virtual std::size_t offset11(int) { return offset = 11; }
  virtual std::size_t offset12(int) { return offset = 12; }   virtual std::size_t offset13(int) { return offset = 13; }   virtual std::size_t offset14(int) { return offset = 14; }
  virtual std::size_t offset15(int) { return offset = 15; }   virtual std::size_t offset16(int) { return offset = 16; }   virtual std::size_t offset17(int) { return offset = 17; }
  virtual std::size_t offset18(int) { return offset = 18; }   virtual std::size_t offset19(int) { return offset = 19; }   virtual std::size_t offset20(int) { return offset = 20; }
  virtual std::size_t offset21(int) { return offset = 21; }   virtual std::size_t offset22(int) { return offset = 22; }   virtual std::size_t offset23(int) { return offset = 23; }
  virtual std::size_t offset24(int) { return offset = 24; }   virtual std::size_t offset25(int) { return offset = 25; }   virtual std::size_t offset26(int) { return offset = 26; }
  virtual std::size_t offset27(int) { return offset = 27; }   virtual std::size_t offset28(int) { return offset = 28; }   virtual std::size_t offset29(int) { return offset = 29; }
  virtual std::size_t offset30(int) { return offset = 30; }   virtual std::size_t offset31(int) { return offset = 31; }   virtual std::size_t offset32(int) { return offset = 32; }
  virtual std::size_t offset33(int) { return offset = 33; }   virtual std::size_t offset34(int) { return offset = 34; }   virtual std::size_t offset35(int) { return offset = 35; }
  virtual std::size_t offset36(int) { return offset = 36; }   virtual std::size_t offset37(int) { return offset = 37; }   virtual std::size_t offset38(int) { return offset = 38; }
  virtual std::size_t offset39(int) { return offset = 39; }   virtual std::size_t offset40(int) { return offset = 40; }   virtual std::size_t offset41(int) { return offset = 41; }
  virtual std::size_t offset42(int) { return offset = 42; }   virtual std::size_t offset43(int) { return offset = 43; }   virtual std::size_t offset44(int) { return offset = 44; }
  virtual std::size_t offset45(int) { return offset = 45; }   virtual std::size_t offset46(int) { return offset = 46; }   virtual std::size_t offset47(int) { return offset = 47; }
  virtual std::size_t offset48(int) { return offset = 48; }   virtual std::size_t offset49(int) { return offset = 49; }   virtual std::size_t offset50(int) { return offset = 50; }
  virtual std::size_t offset51(int) { return offset = 51; }   virtual std::size_t offset52(int) { return offset = 52; }   virtual std::size_t offset53(int) { return offset = 53; }
  virtual std::size_t offset54(int) { return offset = 54; }   virtual std::size_t offset55(int) { return offset = 55; }   virtual std::size_t offset56(int) { return offset = 56; }
  virtual std::size_t offset57(int) { return offset = 57; }   virtual std::size_t offset58(int) { return offset = 58; }   virtual std::size_t offset59(int) { return offset = 59; }
  virtual std::size_t offset60(int) { return offset = 60; }   virtual std::size_t offset61(int) { return offset = 61; }   virtual std::size_t offset62(int) { return offset = 62; }
  virtual std::size_t offset63(int) { return offset = 63; }   virtual std::size_t offset64(int) { return offset = 64; }   virtual std::size_t offset65(int) { return offset = 65; }
  virtual std::size_t offset66(int) { return offset = 66; }   virtual std::size_t offset67(int) { return offset = 67; }   virtual std::size_t offset68(int) { return offset = 68; }
  virtual std::size_t offset69(int) { return offset = 69; }   virtual std::size_t offset70(int) { return offset = 70; }   virtual std::size_t offset71(int) { return offset = 71; }
  virtual std::size_t offset72(int) { return offset = 72; }   virtual std::size_t offset73(int) { return offset = 73; }   virtual std::size_t offset74(int) { return offset = 74; }
  virtual std::size_t offset75(int) { return offset = 75; }   virtual std::size_t offset76(int) { return offset = 76; }   virtual std::size_t offset77(int) { return offset = 77; }
  virtual std::size_t offset78(int) { return offset = 78; }   virtual std::size_t offset79(int) { return offset = 79; }   virtual std::size_t offset80(int) { return offset = 80; }
  virtual std::size_t offset81(int) { return offset = 81; }   virtual std::size_t offset82(int) { return offset = 82; }   virtual std::size_t offset83(int) { return offset = 83; }
  virtual std::size_t offset84(int) { return offset = 84; }   virtual std::size_t offset85(int) { return offset = 85; }   virtual std::size_t offset86(int) { return offset = 86; }
  virtual std::size_t offset87(int) { return offset = 87; }   virtual std::size_t offset88(int) { return offset = 88; }   virtual std::size_t offset89(int) { return offset = 89; }
  virtual std::size_t offset90(int) { return offset = 90; }   virtual std::size_t offset91(int) { return offset = 91; }   virtual std::size_t offset92(int) { return offset = 92; }
  virtual std::size_t offset93(int) { return offset = 93; }   virtual std::size_t offset94(int) { return offset = 94; }   virtual std::size_t offset95(int) { return offset = 95; }
  virtual std::size_t offset96(int) { return offset = 96; }   virtual std::size_t offset97(int) { return offset = 97; }   virtual std::size_t offset98(int) { return offset = 98; }
  virtual std::size_t offset99(int) { return offset = 99; }   virtual std::size_t offset100(int) { return offset = 100; } virtual std::size_t offset101(int) { return offset = 101; }
  virtual std::size_t offset102(int) { return offset = 102; } virtual std::size_t offset103(int) { return offset = 103; } virtual std::size_t offset104(int) { return offset = 104; }
  virtual std::size_t offset105(int) { return offset = 105; } virtual std::size_t offset106(int) { return offset = 106; } virtual std::size_t offset107(int) { return offset = 107; }
  virtual std::size_t offset108(int) { return offset = 108; } virtual std::size_t offset109(int) { return offset = 109; } virtual std::size_t offset110(int) { return offset = 110; }
  virtual std::size_t offset111(int) { return offset = 111; } virtual std::size_t offset112(int) { return offset = 112; } virtual std::size_t offset113(int) { return offset = 113; }
  virtual std::size_t offset114(int) { return offset = 114; } virtual std::size_t offset115(int) { return offset = 115; } virtual std::size_t offset116(int) { return offset = 116; }
  virtual std::size_t offset117(int) { return offset = 117; } virtual std::size_t offset118(int) { return offset = 118; } virtual std::size_t offset119(int) { return offset = 119; }
  virtual std::size_t offset120(int) { return offset = 120; } virtual std::size_t offset121(int) { return offset = 121; } virtual std::size_t offset122(int) { return offset = 122; }
  virtual std::size_t offset123(int) { return offset = 123; } virtual std::size_t offset124(int) { return offset = 124; } virtual std::size_t offset125(int) { return offset = 125; }
  virtual std::size_t offset126(int) { return offset = 126; } virtual std::size_t offset127(int) { return offset = 127; }
  virtual ~virtual_offset() = default;
};
// clang-format on

template <class R, class B, class... TArgs>
inline auto offset(R (B::*f)(TArgs...) const) {
  auto ptr = reinterpret_cast<std::size_t (virtual_offset::*)(int)>(f);
  return (virtual_offset{}.*ptr)(0);
}

template <class R, class B, class... TArgs>
inline auto offset(R (B::*f)(TArgs...)) {
  auto ptr = reinterpret_cast<std::size_t (virtual_offset::*)(int)>(f);
  return (virtual_offset{}.*ptr)(0);
}

template <class T>
inline auto dtor_offset() {
  virtual_offset offset;
  union_cast<T *>(&offset)->~T();
  return offset.offset;
}

template <class T>
inline auto vtable_size() {
  struct derrived : T {
    virtual void vtable_end() {}
  };
  return offset(&derrived::vtable_end);
}

/**
 * Itanium C++ ABI - https://mentorembedded.github.io/cxx-abi/abi.html
 * @tparam T interface type
 */
template <class T>
class vtable {
  static constexpr auto OFFSET_SIZE = 2u;
  static constexpr auto COOKIES_SIZE = 2u;

 public:
  vtable(void *f, void *dtor) : vptr{make_vtable()} {
    for (auto i = 0u; i < vtable_size<T>(); ++i) {
      set(i, f);
    }
    set(dtor);
  }
  vtable(vtable &&) = default;
  vtable(const vtable &) = delete;
  ~vtable() {
    vptr -= OFFSET_SIZE + COOKIES_SIZE;
    delete[] vptr;
  }

  void set(std::size_t offset, void *f) { vptr[offset] = f; }
  void set(void *f) {
    const auto offset = dtor_offset<T>();
    const auto ptr = union_cast<void *>(&vtable<T>::dtor);
    vptr[offset] = f;        // non-deleting dtor
    vptr[offset + 1] = ptr;  // deleting dtor
  }
  auto get(std::size_t offset) const { return vptr[offset]; }

 private:
  auto dtor(int) {
    auto *self = (T *)this;
    auto vt = (vtable *)self;
    auto offset = dtor_offset<T>();
    auto ptr = vt->get(offset);
    void (*f)(T *) = union_cast<void (*)(T *)>(ptr);
    f(self);
    return 0;
  }

  static auto make_vtable() {
    const auto size = vtable_size<T>();
    auto vptr = new void *[size + OFFSET_SIZE + COOKIES_SIZE]{};
    vptr[0] = const_cast<std::type_info *>(&typeid(T));
    vptr += COOKIES_SIZE + OFFSET_SIZE;
    return vptr;
  }

  void **vptr = nullptr;
};

using CallReactionType = internal::CallReaction (*)(const void *);
template <CallReactionType Ptr>
struct GetAccess {
  friend CallReactionType GetCallReaction() { return Ptr; }
};
CallReactionType GetCallReaction();
template struct GetAccess<&Mock::GetReactionOnUninterestingCalls>;

}  // detail

template <class T>
class GMock {
  static_assert(detail::is_complete<T>::value, "T has to be a complete type");
  static_assert(std::is_polymorphic<T>::value,
                "T has to be a polymorphic type");
  static_assert(std::has_virtual_destructor<T>::value,
                "T has to have a virtual destructor");

  detail::vtable<T> vtable;
  detail::byte _[sizeof(T)] = {0};

  void expected() {}

  template <class TName = detail::string<>, class R = void *, class... TArgs>
  R not_expected(TArgs... args) {
    const auto addr = (volatile int *)__builtin_return_address(0) - 1;
    auto *ptr = [this] {
      fs[__PRETTY_FUNCTION__] = std::make_unique<FunctionMocker<R(TArgs...)>>();
      return static_cast<FunctionMocker<R(TArgs...)> *>(
          fs[__PRETTY_FUNCTION__].get());
    }();

    if (internal::CallReaction::kAllow ==
        detail::GetCallReaction()(internal::ImplicitCast_<GMock<T> *>(this))) {
      ptr->SetOwnerAndName(this, __PRETTY_FUNCTION__);
    } else {
      const auto info = [](const std::string &file, int line) {
        std::ifstream input(file);
        std::string buf;
        for (auto i = 0; i < line; ++i) getline(input, buf);
        detail::trim(buf);
        return buf + "\n\t       At: [" + detail::basename(file) + ":" +
               std::to_string(line) + "]";
      };
      const auto al = detail::addr2line((void *)addr);
      msgs.emplace_back(info(al.first, al.second) +
                        "\n\t     From: " + detail::call_stack("\n\t\t   ", 2));
      ptr->SetOwnerAndName(this,
                           std::is_same<TName, void>::value
                               ? msgs.back().c_str()
                               : TName::c_str());
    }
    return ptr->Invoke(args...);
  }

  template <class TName, class R, class... TArgs>
  decltype(auto) gmock_call_impl(
      int offset, const detail::identity_t<Matcher<TArgs>> &... args) {
    vtable.set(offset, detail::union_cast<void *>(
                           &GMock::template original_call<TName, R, TArgs...>));

    const auto it = fs.find(TName::c_str());
    auto *ptr = [&it, this] {
      if (it == fs.end()) {
        fs[TName::c_str()] = std::make_unique<FunctionMocker<R(TArgs...)>>();
        return static_cast<FunctionMocker<R(TArgs...)> *>(
            fs[TName::c_str()].get());
      }
      return static_cast<FunctionMocker<R(TArgs...)> *>(it->second.get());
    }();

    ptr->RegisterOwner(this);
    return ptr->With(args...);
  }

  template <class TName, class R, class... TArgs>
  R original_call(TArgs... args) {
    if (fs.find(TName::c_str()) != fs.end()) {
      auto *f =
          static_cast<FunctionMocker<R(TArgs...)> *>(fs[TName::c_str()].get());
      f->SetOwnerAndName(this, TName::c_str());
      return f->Invoke(args...);
    }

    return not_expected<TName, R, TArgs...>(args...);
  }

  template <class TName, class R, class B, class... TArgs>
  void defer_call(R (B::*f)(TArgs...)) {
    vtable.set(detail::offset(f),
               detail::union_cast<void *>(
                   &GMock::template original_defer_call<TName, R, TArgs...>));
  }

  template <class TName, class R, class B, class... TArgs>
  void defer_call(R (B::*f)(TArgs...) const) {
    vtable.set(detail::offset(f),
               detail::union_cast<void *>(
                   &GMock::template original_defer_call<TName, R, TArgs...>));
  }

  template <class TName, class R, class... TArgs>
  void original_defer_call(TArgs... args) {
    calls.push_back([=] { original_call<TName, R>(args...); });
  }

 public:
  using type = T;

  GMock()
      : vtable{detail::union_cast<void *>(&GMock::template not_expected<>),
               detail::union_cast<void *>(&GMock::expected)} {}
  GMock(const GMock &) = delete;
  GMock(GMock &&) = default;
  ~GMock() noexcept {
    for (const auto &call : calls) {
      call();
    }
  }

  template <class... Ts>
  GMock(const Ts &... call) : GMock() {
    using swallow = int[];
    (void)swallow{0, (defer_call<decltype(call.first)>(call.second), 0)...};
  }

  template <class TName, class R, class B, class... TArgs>
  decltype(auto) gmock_call(
      R (B::*f)(TArgs...), const detail::identity_t<Matcher<TArgs>> &... args) {
    return gmock_call_impl<TName, R, TArgs...>(detail::offset(f), args...);
  }

  template <class TName, class R, class B, class... TArgs>
  decltype(auto) gmock_call(
      R (B::*f)(TArgs...) const,
      const typename detail::identity_t<Matcher<TArgs>> &... args) {
    return gmock_call_impl<TName, R, TArgs...>(detail::offset(f), args...);
  }

  T &object() { return reinterpret_cast<T &>(*this); }
  const T &object() const { return reinterpret_cast<const T &>(*this); }
  explicit operator T &() { return object(); }
  explicit operator const T &() const { return object(); }

 private:
  std::unordered_map<std::string,
                     std::unique_ptr<internal::UntypedFunctionMockerBase>>
      fs;
  std::vector<std::string> msgs;
  std::vector<std::function<void()>> calls;
};
}  // v1

template <class T>
class NiceMock<GMock<T>> final : public GMock<T> {
 public:
  template <class... Ts>
  NiceMock(Ts &&... ts) : GMock<T>{std::forward<Ts>(ts)...} {
    Mock::AllowUninterestingCalls(internal::ImplicitCast_<GMock<T> *>(this));
  }

  NiceMock(NiceMock &&) = default;
  NiceMock(const NiceMock &) = delete;
  NiceMock() {
    Mock::AllowUninterestingCalls(internal::ImplicitCast_<GMock<T> *>(this));
  }
  ~NiceMock() {
    Mock::UnregisterCallReaction(internal::ImplicitCast_<GMock<T> *>(this));
  }
};

template <class T>
class StrictMock<GMock<T>> final : public GMock<T> {
 public:
  template <class... Ts>
  StrictMock(Ts &&... ts) : GMock<T>{std::forward<Ts>(ts)...} {
    Mock::FailUninterestingCalls(internal::ImplicitCast_<GMock<T> *>(this));
  }
  StrictMock(StrictMock &&) = default;
  StrictMock(const StrictMock &) = delete;
  StrictMock() {
    Mock::FailUninterestingCalls(internal::ImplicitCast_<GMock<T> *>(this));
  }
  ~StrictMock() {
    Mock::UnregisterCallReaction(internal::ImplicitCast_<GMock<T> *>(this));
  }
};

inline namespace v1 {
template <class T>
using NaggyGMock = GMock<T>;

template <class T>
using StrictGMock = StrictMock<GMock<T>>;

template <class T>
using NiceGMock = NiceMock<GMock<T>>;
}  // v1
}  // testing

namespace std {
template <class T, class TDeleter>
auto move(unique_ptr<testing::GMock<T>, TDeleter> &mock) noexcept {
  return unique_ptr<T>{reinterpret_cast<T *>(mock.get())};  // it's not release
}
template <class T, class TDeleter>
auto move(unique_ptr<testing::StrictGMock<T>, TDeleter> &mock) noexcept {
  return unique_ptr<T>{reinterpret_cast<T *>(mock.get())};  // it's not release
}
template <class T, class TDeleter>
auto move(unique_ptr<testing::NiceGMock<T>, TDeleter> &mock) noexcept {
  return unique_ptr<T>{reinterpret_cast<T *>(mock.get())};  // it's not release
}

template <class T, class U>
auto static_pointer_cast(
    const std::shared_ptr<testing::GMock<U>> &mock) noexcept {
  return std::shared_ptr<T>{
      mock, reinterpret_cast<T *>(mock.get())};  // it's not release
}
template <class T, class U>
auto static_pointer_cast(
    const std::shared_ptr<testing::StrictGMock<U>> &mock) noexcept {
  return std::shared_ptr<T>{
      mock, reinterpret_cast<T *>(mock.get())};  // it's not release
}
template <class T, class U>
auto static_pointer_cast(
    const std::shared_ptr<testing::NiceGMock<U>> &mock) noexcept {
  return std::shared_ptr<T>{
      mock, reinterpret_cast<T *>(mock.get())};  // it's not release
}
}  // std

namespace testing {
template <class R>
auto Return(const std::shared_ptr<GMock<R>> &value) {
  return internal::ReturnAction<std::shared_ptr<R>>(
      std::move(std::static_pointer_cast<R>(value)));
}
template <class R>
auto Return(const std::shared_ptr<StrictGMock<R>> &value) {
  return internal::ReturnAction<std::shared_ptr<R>>(
      std::move(std::static_pointer_cast<R>(value)));
}
template <class R>
auto Return(const std::shared_ptr<NiceGMock<R>> &value) {
  return internal::ReturnAction<std::shared_ptr<R>>(
      std::move(std::static_pointer_cast<R>(value)));
}

template <class R>
auto Return(GMock<R> *value) {
  return internal::ReturnAction<R *>(std::move(reinterpret_cast<R *>(value)));
}
template <class R>
auto Return(StrictGMock<R> *value) {
  return internal::ReturnAction<R *>(std::move(reinterpret_cast<R *>(value)));
}
template <class R>
auto Return(NiceGMock<R> *value) {
  return internal::ReturnAction<R *>(std::move(reinterpret_cast<R *>(value)));
}

template <class R>
inline auto ReturnRef(GMock<R> &x) {
  return internal::ReturnRefAction<R>(static_cast<R &>(x));
}
template <class R>
inline auto ReturnRef(StrictGMock<R> &x) {
  return internal::ReturnRefAction<R>(static_cast<R &>(x));
}
template <class R>
inline auto ReturnRef(NiceGMock<R> &x) {
  return internal::ReturnRefAction<R>(static_cast<R &>(x));
}

template <class T>
inline auto Ref(GMock<T> &x) {
  return internal::RefMatcher<T &>(static_cast<T &>(x));
}
template <class T>
inline auto Ref(StrictGMock<T> &x) {
  return internal::RefMatcher<T &>(static_cast<T &>(x));
}
template <class T>
inline auto Ref(NiceGMock<T> &x) {
  return internal::RefMatcher<T &>(static_cast<T &>(x));
}

template <class T>
inline auto ByRef(GMock<T> &x) {
  return internal::ReferenceWrapper<T>(static_cast<T &>(x));
}
template <class T>
inline auto ByRef(StrictGMock<T> &x) {
  return internal::ReferenceWrapper<T>(static_cast<T &>(x));
}
template <class T>
inline auto ByRef(NiceGMock<T> &x) {
  return internal::ReferenceWrapper<T>(static_cast<T &>(x));
}

inline namespace v1 {
namespace detail {
template <class T, class TMock>
struct Object {
  using mock_t = typename T::type;

  operator const mock_t &() const { return mock.object(); }
  operator mock_t &() { return mock.object(); }
  operator mock_t *() { return &mock.object(); }
  operator const mock_t *() const { return &mock.object(); }

  TMock &mock;
};

template <class T, class TMock>
struct Object<std::shared_ptr<T>, TMock> {
  using mock_t = typename T::type;

  operator const mock_t &() const { return mock->object(); }
  operator mock_t &() { return mock->object(); }
  operator mock_t *() { return &mock->object(); }
  operator const mock_t *() const { return &mock->object(); }
  operator std::shared_ptr<mock_t>() {
    return std::static_pointer_cast<mock_t>(mock);
  }

  TMock &mock;
};
}
}  // detail::v1

template <class TMock>
auto object(TMock &mock) {
  return detail::Object<std::decay_t<TMock>, TMock>{mock};
}
template <class TMock>
auto object(TMock *mock) {
  return detail::Object<std::decay_t<TMock>, TMock>{*mock};
}
}  // testing

#if defined(__clang__)
#pragma clang optimize on
#elif defined(__GNUC__)
#pragma GCC pop_options
#endif

#define __GMOCK_QNAME(...) \
  decltype(__GUNIT_CAT(#__VA_ARGS__, _gtest_string)) __GUNIT_IGNORE
#define __GMOCK_FUNCTION(a, b) b __GUNIT_IGNORE
#define __GMOCK_NAME(...) \
  __GUNIT_CAT(__GMOCK_NAME_, __GUNIT_SIZE(__VA_ARGS__))(__VA_ARGS__)
#define __GMOCK_NAME_1(a) a __GUNIT_IGNORE
#define __GMOCK_NAME_2(a, b) a __GUNIT_IGNORE
#define __GMOCK_OVERLOAD_CALL(...) __GUNIT_SIZE(__VA_ARGS__) __GUNIT_IGNORE
#define __GMOCK_OVERLOAD_CAST_IMPL_1(obj, call)
#define __GMOCK_OVERLOAD_CAST_IMPL_2(obj, call)                          \
  (::testing::detail::function_type_t<std::decay_t<decltype(obj)>::type, \
                                      __GMOCK_FUNCTION call>)
#define __GMOCK_INTERNAL(...)               \
  __GUNIT_IF(__GUNIT_IS_EMPTY(__VA_ARGS__)) \
  (__GUNIT_IGNORE, __GUNIT_COMMA)() __VA_ARGS__
#define __GMOCK_CALL(...) __GMOCK_INTERNAL

#undef EXPECT_CALL
#define EXPECT_CALL(obj, call) \
  __GUNIT_CAT(__GMOCK_EXPECT_CALL_, __GUNIT_IBP(call))(obj, call, call)
#define __GMOCK_EXPECT_CALL_0(obj, _, call) GMOCK_EXPECT_CALL_IMPL_(obj, call)
#define __GMOCK_EXPECT_CALL_1(obj, qcall, call)                              \
  ((obj).template gmock_call<__GMOCK_QNAME call>(                            \
       __GUNIT_CAT(__GMOCK_OVERLOAD_CAST_IMPL_, __GMOCK_OVERLOAD_CALL call)( \
           obj, call) &                                                      \
       std::decay_t<decltype(obj)>::type::__GMOCK_NAME call __GMOCK_CALL     \
           call))                                                            \
      .InternalExpectedAt(__FILE__, __LINE__, #obj, #qcall)

#define EXPECT_INVOKE(obj, f, ...) \
  __GUNIT_CAT(__GMOCK_EXPECT_INVOKE_IMPL_, __GUNIT_IBP(f))(obj, f, __VA_ARGS__)
#define __GMOCK_EXPECT_INVOKE_IMPL_0(obj, f, ...)             \
  ::testing::detail::constexpr_if(                            \
      ::testing::detail::is_valid(                            \
          [](auto &&x) -> decltype(x.f(__VA_ARGS__)) {}),     \
      [](auto &&x) -> decltype(auto) {                        \
        return GMOCK_EXPECT_CALL_IMPL_(x, f(__VA_ARGS__));    \
      },                                                      \
      [](auto &&x) -> decltype(auto) {                        \
        return __GMOCK_EXPECT_CALL_1(                         \
            x, f(__VA_ARGS__),                                \
            __GUNIT_IF(__GUNIT_IBP(f))(f, (f))(__VA_ARGS__)); \
      })(obj)
#define __GMOCK_EXPECT_INVOKE_IMPL_1(obj, f, ...) \
  __GMOCK_EXPECT_CALL_1(obj, f(__VA_ARGS__), f(__VA_ARGS__))

#undef ON_CALL
#define ON_CALL(obj, call) \
  __GUNIT_CAT(__GMOCK_ON_CALL_, __GUNIT_IBP(call))(obj, call, call)
#define __GMOCK_ON_CALL_0(obj, _, call) GMOCK_ON_CALL_IMPL_(obj, call)
#define __GMOCK_ON_CALL_1(obj, qcall, call)                                  \
  ((obj).template gmock_call<__GMOCK_QNAME call>(                            \
       __GUNIT_CAT(__GMOCK_OVERLOAD_CAST_IMPL_, __GMOCK_OVERLOAD_CALL call)( \
           obj, call) &                                                      \
       std::decay_t<decltype(obj)>::type::__GMOCK_NAME call __GMOCK_CALL     \
           call))                                                            \
      .InternalDefaultActionSetAt(__FILE__, __LINE__, #obj, #qcall)

#define ON_INVOKE(obj, f, ...) \
  __GUNIT_CAT(__GMOCK_ON_INVOKE_IMPL_, __GUNIT_IBP(f))(obj, f, __VA_ARGS__)
#define __GMOCK_ON_INVOKE_IMPL_0(obj, f, ...)                 \
  ::testing::detail::constexpr_if(                            \
      ::testing::detail::is_valid(                            \
          [](auto &&x) -> decltype(x.f(__VA_ARGS__)) {}),     \
      [](auto &&x) -> decltype(auto) {                        \
        return GMOCK_ON_CALL_IMPL_(x, f(__VA_ARGS__));        \
      },                                                      \
      [](auto &&x) -> decltype(auto) {                        \
        return __GMOCK_ON_CALL_1(                             \
            x, f(__VA_ARGS__),                                \
            __GUNIT_IF(__GUNIT_IBP(f))(f, (f))(__VA_ARGS__)); \
      })(obj)
#define __GMOCK_ON_INVOKE_IMPL_1(obj, f, ...) \
  __GMOCK_ON_CALL_1(obj, f(__VA_ARGS__), f(__VA_ARGS__))

#define __GMOCK_DEFER_CALLS_IMPL(I, f) std::make_pair(#f##_gtest_string, &I::f)
#define __GMOCK_DEFER_CALLS_IMPL_10(I, f1, f2, f3, f4, f5, f6, f7, f8, f9, \
                                    f10)                                   \
  __GMOCK_DEFER_CALLS_IMPL_9(I, f1, f2, f3, f4, f5, f6, f7, f8, f9)        \
  , __GMOCK_DEFER_CALLS_IMPL(I, f10)
#define __GMOCK_DEFER_CALLS_IMPL_9(I, f1, f2, f3, f4, f5, f6, f7, f8, f9) \
  __GMOCK_DEFER_CALLS_IMPL_8(I, f1, f2, f3, f4, f5, f6, f7, f8)           \
  , __GMOCK_DEFER_CALLS_IMPL(I, f9)
#define __GMOCK_DEFER_CALLS_IMPL_8(I, f1, f2, f3, f4, f5, f6, f7, f8) \
  __GMOCK_DEFER_CALLS_IMPL_7(I, f1, f2, f3, f4, f5, f6, f7)           \
  , __GMOCK_DEFER_CALLS_IMPL(I, f8)
#define __GMOCK_DEFER_CALLS_IMPL_7(I, f1, f2, f3, f4, f5, f6, f7) \
  __GMOCK_DEFER_CALLS_IMPL_6(I, f1, f2, f3, f4, f5, f6)           \
  , __GMOCK_DEFER_CALLS_IMPL(I, f7)
#define __GMOCK_DEFER_CALLS_IMPL_6(I, f1, f2, f3, f4, f5, f6) \
  __GMOCK_DEFER_CALLS_IMPL_5(I, f1, f2, f3, f4, f5)           \
  , __GMOCK_DEFER_CALLS_IMPL(I, f6)
#define __GMOCK_DEFER_CALLS_IMPL_5(I, f1, f2, f3, f4, f5) \
  __GMOCK_DEFER_CALLS_IMPL_4(I, f1, f2, f3, f4), __GMOCK_DEFER_CALLS_IMPL(I, f5)
#define __GMOCK_DEFER_CALLS_IMPL_4(I, f1, f2, f3, f4) \
  __GMOCK_DEFER_CALLS_IMPL_3(I, f1, f2, f3), __GMOCK_DEFER_CALLS_IMPL(I, f4)
#define __GMOCK_DEFER_CALLS_IMPL_3(I, f1, f2, f3) \
  __GMOCK_DEFER_CALLS_IMPL_2(I, f1, f2), __GMOCK_DEFER_CALLS_IMPL(I, f3)
#define __GMOCK_DEFER_CALLS_IMPL_2(I, f1, f2) \
  __GMOCK_DEFER_CALLS_IMPL_1(I, f1), __GMOCK_DEFER_CALLS_IMPL(I, f2)
#define __GMOCK_DEFER_CALLS_IMPL_1(I, f1) __GMOCK_DEFER_CALLS_IMPL(I, f1)

#define DEFER_CALLS(I, ...)                                         \
  __GUNIT_CAT(__GMOCK_DEFER_CALLS_IMPL_, __GUNIT_SIZE(__VA_ARGS__)) \
  (I, __VA_ARGS__)
#define EXPECTED_CALL EXPECT_CALL
#define EXPECTED_INVOKE EXPECT_INVOKE
