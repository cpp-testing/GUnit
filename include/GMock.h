//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <gmock/gmock.h>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace testing {
inline namespace v1 {
namespace detail {

template <class T>
struct identity {
  using type = T;
};

template <class T>
using identity_t = typename identity<T>::type;

template <class TDst, class TSrc>
inline TDst union_cast(TSrc src) {
  union {
    TSrc src;
    TDst dst;
  } u;
  u.src = src;
  return u.dst;
}

template <char... Chrs>
struct string {
  static auto c_str() {
    static constexpr char str[] = {Chrs..., 0};
    return str;
  }
};

template <class T>
inline std::size_t vf_offset(T f) {
  union {  // GNU
    T f;
    struct {
      unsigned long value;
      unsigned long baseoffs;
    } u;
  } conv;
  conv.f = f;

  static constexpr auto IsItaniumABI = 1;
  return (conv.u.value & IsItaniumABI) ? conv.u.value / sizeof(void*) : std::size_t{};
}

template <class T>
inline std::size_t vf_dtor_offset() {
  // clang-format off
  struct vptr_index {
    std::size_t offset;
    virtual std::size_t f0(std::size_t) { return offset = 0; }     virtual std::size_t f1(std::size_t) { return offset = 1; }     virtual std::size_t f2(std::size_t) { return offset = 2; }      virtual std::size_t f3(std::size_t) { return offset = 3; }
    virtual std::size_t f4(std::size_t) { return offset = 4; }     virtual std::size_t f5(std::size_t) { return offset = 5; }     virtual std::size_t f6(std::size_t) { return offset = 6; }      virtual std::size_t f7(std::size_t) { return offset = 7; }
    virtual std::size_t f8(std::size_t) { return offset = 8; }     virtual std::size_t f9(std::size_t) { return offset = 9; }     virtual std::size_t f10(std::size_t) { return offset = 10; }    virtual std::size_t f11(std::size_t) { return offset = 11; }
    virtual std::size_t f12(std::size_t) { return offset = 12; }   virtual std::size_t f13(std::size_t) { return offset = 13; }   virtual std::size_t f14(std::size_t) { return offset = 14; }    virtual std::size_t f15(std::size_t) { return offset = 15; }
    virtual std::size_t f16(std::size_t) { return offset = 16; }   virtual std::size_t f17(std::size_t) { return offset = 17; }   virtual std::size_t f18(std::size_t) { return offset = 18; }    virtual std::size_t f19(std::size_t) { return offset = 19; }
    virtual std::size_t f20(std::size_t) { return offset = 20; }   virtual std::size_t f21(std::size_t) { return offset = 21; }   virtual std::size_t f22(std::size_t) { return offset = 22; }    virtual std::size_t f23(std::size_t) { return offset = 23; }
    virtual std::size_t f24(std::size_t) { return offset = 24; }   virtual std::size_t f25(std::size_t) { return offset = 25; }   virtual std::size_t f26(std::size_t) { return offset = 26; }    virtual std::size_t f27(std::size_t) { return offset = 27; }
    virtual std::size_t f28(std::size_t) { return offset = 28; }   virtual std::size_t f29(std::size_t) { return offset = 29; }   virtual std::size_t f30(std::size_t) { return offset = 30; }    virtual std::size_t f31(std::size_t) { return offset = 31; }
    virtual std::size_t f32(std::size_t) { return offset = 32; }   virtual std::size_t f33(std::size_t) { return offset = 33; }   virtual std::size_t f34(std::size_t) { return offset = 34; }    virtual std::size_t f35(std::size_t) { return offset = 35; }
    virtual std::size_t f36(std::size_t) { return offset = 36; }   virtual std::size_t f37(std::size_t) { return offset = 37; }   virtual std::size_t f38(std::size_t) { return offset = 38; }    virtual std::size_t f39(std::size_t) { return offset = 39; }
    virtual std::size_t f40(std::size_t) { return offset = 40; }   virtual std::size_t f41(std::size_t) { return offset = 41; }   virtual std::size_t f42(std::size_t) { return offset = 42; }    virtual std::size_t f43(std::size_t) { return offset = 43; }
    virtual std::size_t f44(std::size_t) { return offset = 44; }   virtual std::size_t f45(std::size_t) { return offset = 45; }   virtual std::size_t f46(std::size_t) { return offset = 46; }    virtual std::size_t f47(std::size_t) { return offset = 47; }
    virtual std::size_t f48(std::size_t) { return offset = 48; }   virtual std::size_t f49(std::size_t) { return offset = 49; }   virtual std::size_t f50(std::size_t) { return offset = 50; }    virtual std::size_t f51(std::size_t) { return offset = 51; }
    virtual std::size_t f52(std::size_t) { return offset = 52; }   virtual std::size_t f53(std::size_t) { return offset = 53; }   virtual std::size_t f54(std::size_t) { return offset = 54; }    virtual std::size_t f55(std::size_t) { return offset = 55; }
    virtual std::size_t f56(std::size_t) { return offset = 56; }   virtual std::size_t f57(std::size_t) { return offset = 57; }   virtual std::size_t f58(std::size_t) { return offset = 58; }    virtual std::size_t f59(std::size_t) { return offset = 59; }
    virtual std::size_t f60(std::size_t) { return offset = 60; }   virtual std::size_t f61(std::size_t) { return offset = 61; }   virtual std::size_t f62(std::size_t) { return offset = 62; }    virtual std::size_t f63(std::size_t) { return offset = 63; }
    virtual std::size_t f64(std::size_t) { return offset = 64; }   virtual std::size_t f65(std::size_t) { return offset = 65; }   virtual std::size_t f66(std::size_t) { return offset = 66; }    virtual std::size_t f67(std::size_t) { return offset = 67; }
    virtual std::size_t f68(std::size_t) { return offset = 68; }   virtual std::size_t f69(std::size_t) { return offset = 69; }   virtual std::size_t f70(std::size_t) { return offset = 70; }    virtual std::size_t f71(std::size_t) { return offset = 71; }
    virtual std::size_t f72(std::size_t) { return offset = 72; }   virtual std::size_t f73(std::size_t) { return offset = 73; }   virtual std::size_t f74(std::size_t) { return offset = 74; }    virtual std::size_t f75(std::size_t) { return offset = 75; }
    virtual std::size_t f76(std::size_t) { return offset = 76; }   virtual std::size_t f77(std::size_t) { return offset = 77; }   virtual std::size_t f78(std::size_t) { return offset = 78; }    virtual std::size_t f79(std::size_t) { return offset = 79; }
    virtual std::size_t f80(std::size_t) { return offset = 80; }   virtual std::size_t f81(std::size_t) { return offset = 81; }   virtual std::size_t f82(std::size_t) { return offset = 82; }    virtual std::size_t f83(std::size_t) { return offset = 83; }
    virtual std::size_t f84(std::size_t) { return offset = 84; }   virtual std::size_t f85(std::size_t) { return offset = 85; }   virtual std::size_t f86(std::size_t) { return offset = 86; }    virtual std::size_t f87(std::size_t) { return offset = 87; }
    virtual std::size_t f88(std::size_t) { return offset = 88; }   virtual std::size_t f89(std::size_t) { return offset = 89; }   virtual std::size_t f90(std::size_t) { return offset = 90; }    virtual std::size_t f91(std::size_t) { return offset = 91; }
    virtual std::size_t f92(std::size_t) { return offset = 92; }   virtual std::size_t f93(std::size_t) { return offset = 93; }   virtual std::size_t f94(std::size_t) { return offset = 94; }    virtual std::size_t f95(std::size_t) { return offset = 95; }
    virtual std::size_t f96(std::size_t) { return offset = 96; }   virtual std::size_t f97(std::size_t) { return offset = 97; }   virtual std::size_t f98(std::size_t) { return offset = 98; }    virtual std::size_t f99(std::size_t) { return offset = 99; }
    virtual std::size_t f100(std::size_t) { return offset = 100; } virtual std::size_t f101(std::size_t) { return offset = 101; } virtual std::size_t f102(std::size_t) { return offset = 102; }    virtual std::size_t f103(std::size_t) { return offset = 103; }
    virtual std::size_t f104(std::size_t) { return offset = 104; } virtual std::size_t f105(std::size_t) { return offset = 105; } virtual std::size_t f106(std::size_t) { return offset = 106; }    virtual std::size_t f107(std::size_t) { return offset = 107; }
    virtual std::size_t f108(std::size_t) { return offset = 108; } virtual std::size_t f109(std::size_t) { return offset = 109; } virtual std::size_t f110(std::size_t) { return offset = 110; }    virtual std::size_t f111(std::size_t) { return offset = 111; }
    virtual std::size_t f112(std::size_t) { return offset = 112; } virtual std::size_t f113(std::size_t) { return offset = 113; } virtual std::size_t f114(std::size_t) { return offset = 114; }    virtual std::size_t f115(std::size_t) { return offset = 115; }
    virtual std::size_t f116(std::size_t) { return offset = 116; } virtual std::size_t f117(std::size_t) { return offset = 117; } virtual std::size_t f118(std::size_t) { return offset = 118; }    virtual std::size_t f119(std::size_t) { return offset = 119; }
    virtual std::size_t f120(std::size_t) { return offset = 120; } virtual std::size_t f121(std::size_t) { return offset = 121; } virtual std::size_t f122(std::size_t) { return offset = 122; }    virtual std::size_t f123(std::size_t) { return offset = 123; }
    virtual std::size_t f124(std::size_t) { return offset = 124; } virtual std::size_t f125(std::size_t) { return offset = 125; } virtual std::size_t f126(std::size_t) { return offset = 126; }    virtual std::size_t f127(std::size_t) { return offset = 127; }
    virtual ~vptr_index() = default;
  };
  // clang-format on

  vptr_index vptr;
  ((T*)&vptr)->~T();
  return vptr.offset;
}

}  // detail

template <class T>
class GMock {
  static_assert(std::is_polymorphic<T>::value, "T has to be a polymorphic type");
  static_assert(std::has_virtual_destructor<T>::value, "T has to have a virtual destructor");
  static constexpr auto VPTR_LIMIT_SIZE = 128;

  unsigned char _[sizeof(T)] = {0};
  void* old_vptr = nullptr;
  void (*vptr[VPTR_LIMIT_SIZE])() = {0};

  void expected() {}
  void* not_expected() {
    auto* ptr = [this] {
      fs[__PRETTY_FUNCTION__] = std::make_unique<FunctionMocker<void*()>>();
      return static_cast<FunctionMocker<void*()>*>(fs[__PRETTY_FUNCTION__].get());
    }();
    ptr->SetOwnerAndName(this, __PRETTY_FUNCTION__);
    return ptr->Invoke();
  }

  void* update_vptr(void* new_vptr) {
    void* vptr = *(void**)this;
    *(void**)this = new_vptr;
    return vptr;
  }

  template <class TName, class R, class... TArgs>
  decltype(auto) gmock_call_impl(int offset, const detail::identity_t<Matcher<TArgs>>&... args) {
    vptr[offset] = detail::union_cast<void (*)()>(&GMock::template original_call<TName, R, TArgs...>);

    const auto it = fs.find(TName::c_str());
    auto* ptr = [&it, this] {
      if (it == fs.end()) {
        fs[TName::c_str()] = std::make_unique<FunctionMocker<R(TArgs...)>>();
        return static_cast<FunctionMocker<R(TArgs...)>*>(fs[TName::c_str()].get());
      }
      return static_cast<FunctionMocker<R(TArgs...)>*>(it->second.get());
    }();

    ptr->RegisterOwner(this);
    return ptr->With(args...);
  }

  template <class TName, class R, class... TArgs>
  R original_call(TArgs... args) {
    auto* f = static_cast<FunctionMocker<R(TArgs...)>*>(fs[TName::c_str()].get());
    f->SetOwnerAndName(this, TName::c_str());
    return f->Invoke(args...);
  }

  void init(const std::size_t dtor_offset) {
    for (auto i = 0u; i < VPTR_LIMIT_SIZE; ++i) {
      vptr[i] = i == dtor_offset || i == dtor_offset + 1 /*GNU*/ ? detail::union_cast<void (*)()>(&GMock::expected) :  // dtor
                    detail::union_cast<void (*)()>(&GMock::not_expected);
    }
  }

 public:
  using type = T;

  GMock() {
    init(detail::vf_dtor_offset<T>());
    old_vptr = update_vptr(vptr);
  }

  template <class TName, class R, class B, class... TArgs>
  decltype(auto) gmock_call(R (B::*f)(TArgs...), const detail::identity_t<Matcher<TArgs>>&... args) {
    return gmock_call_impl<TName, R, TArgs...>(detail::vf_offset(f), args...);
  }

  template <class TName, class R, class B, class... TArgs>
  decltype(auto) gmock_call(R (B::*f)(TArgs...) const, const typename detail::identity_t<Matcher<TArgs>>&... args) {
    return gmock_call_impl<TName, R, TArgs...>(detail::vf_offset(f), args...);
  }

  T* operator&() { return reinterpret_cast<T*>(this); }
  operator T&() { return reinterpret_cast<T&>(*this); }

 private:
  std::unordered_map<std::string, std::unique_ptr<internal::UntypedFunctionMockerBase>> fs;
};

template <class T>
using StrictGMock = StrictMock<GMock<T>>;

template <class T>
using NiceGMock = NiceMock<GMock<T>>;

}  // v1
}  // testing

namespace std {

template <class T, class TDeleter>
auto move(unique_ptr<::testing::GMock<T>, TDeleter>& mock) noexcept {
  return unique_ptr<T>{reinterpret_cast<T*>(mock.get())};
}

template <class T, class U>
auto static_pointer_cast(const std::shared_ptr<testing::GMock<U>>& mock) noexcept {
  return std::shared_ptr<T>{mock, reinterpret_cast<T*>(mock.get())};
}
}  // std

#define __GMOCK_VPTR_COMMA() ,
#define __GMOCK_VPTR_IGNORE(...)
#define __GMOCK_VPTR_NAME(...) __VA_ARGS__ __GMOCK_VPTR_IGNORE
#define __GMOCK_VPTR_QNAME(...) ::testing::detail::string<__GMOCK_VPTR_STR_IMPL_32(#__VA_ARGS__, 0), 0> __GMOCK_VPTR_IGNORE
#define __GMOCK_VPTR_INTERNAL(...)                  \
  __GMOCK_VPTR_IF(__BOOST_DI_IS_EMPTY(__VA_ARGS__)) \
  (__GMOCK_VPTR_IGNORE, __GMOCK_VPTR_COMMA)() __VA_ARGS__
#define __GMOCK_VPTR_CALL(...) __GMOCK_VPTR_INTERNAL
#define __GMOCK_VPTR_PRIMITIVE_CAT(arg, ...) arg##__VA_ARGS__
#define __GMOCK_VPTR_CAT(arg, ...) __GMOCK_VPTR_PRIMITIVE_CAT(arg, __VA_ARGS__)
#define __GMOCK_VPTR_IBP_SPLIT(i, ...) __GMOCK_VPTR_PRIMITIVE_CAT(__GMOCK_VPTR_IBP_SPLIT_, i)(__VA_ARGS__)
#define __GMOCK_VPTR_IBP_SPLIT_0(a, ...) a
#define __GMOCK_VPTR_IBP_SPLIT_1(a, ...) __VA_ARGS__
#define __GMOCK_VPTR_IBP_IS_VARIADIC_C(...) 1
#define __GMOCK_VPTR_IBP_IS_VARIADIC_R_1 1,
#define __GMOCK_VPTR_IBP_IS_VARIADIC_R___GMOCK_VPTR_IBP_IS_VARIADIC_C 0,
#define __GMOCK_VPTR_IBP(...) \
  __GMOCK_VPTR_IBP_SPLIT(0, __GMOCK_VPTR_CAT(__GMOCK_VPTR_IBP_IS_VARIADIC_R_, __GMOCK_VPTR_IBP_IS_VARIADIC_C __VA_ARGS__))
#define __BOOST_DI_IS_EMPTY(...)                           \
  __GMOCK_VPTR_IS_EMPTY_IIF(__GMOCK_VPTR_IBP(__VA_ARGS__)) \
  (__GMOCK_VPTR_IS_EMPTY_GEN_ZERO, __GMOCK_VPTR_IS_EMPTY_PROCESS)(__VA_ARGS__)
#define __GMOCK_VPTR_IS_EMPTY_PRIMITIVE_CAT(a, b) a##b
#define __GMOCK_VPTR_IS_EMPTY_IIF(bit) __GMOCK_VPTR_IS_EMPTY_PRIMITIVE_CAT(__GMOCK_VPTR_IS_EMPTY_IIF_, bit)
#define __GMOCK_VPTR_IS_EMPTY_NON_FUNCTION_C(...) ()
#define __GMOCK_VPTR_IS_EMPTY_GEN_ZERO(...) 0
#define __GMOCK_VPTR_IS_EMPTY_IIF_0(t, b) b
#define __GMOCK_VPTR_IS_EMPTY_IIF_1(t, b) t
#define __GMOCK_VPTR_IS_EMPTY_PROCESS(...) __GMOCK_VPTR_IBP(__GMOCK_VPTR_IS_EMPTY_NON_FUNCTION_C __VA_ARGS__())
#define __GMOCK_VPTR_IIF(c) __GMOCK_VPTR_PRIMITIVE_CAT(__GMOCK_VPTR_IIF_, c)
#define __GMOCK_VPTR_IIF_0(t, ...) __VA_ARGS__
#define __GMOCK_VPTR_IIF_1(t, ...) t
#define __GMOCK_VPTR_IF(c) __GMOCK_VPTR_IIF(c)
#define __GMOCK_VPTR_STR_IMPL_1(str, i) (sizeof(str) > (i) ? str[(i)] : 0)
#define __GMOCK_VPTR_STR_IMPL_4(str, i) \
  __GMOCK_VPTR_STR_IMPL_1(str, i + 0)   \
  , __GMOCK_VPTR_STR_IMPL_1(str, i + 1), __GMOCK_VPTR_STR_IMPL_1(str, i + 2), __GMOCK_VPTR_STR_IMPL_1(str, i + 3)
#define __GMOCK_VPTR_STR_IMPL_16(str, i) \
  __GMOCK_VPTR_STR_IMPL_4(str, i + 0)    \
  , __GMOCK_VPTR_STR_IMPL_4(str, i + 4), __GMOCK_VPTR_STR_IMPL_4(str, i + 8), __GMOCK_VPTR_STR_IMPL_4(str, i + 12)
#define __GMOCK_VPTR_STR_IMPL_32(str, i) \
  __GMOCK_VPTR_STR_IMPL_16(str, i + 0)   \
  , __GMOCK_VPTR_STR_IMPL_16(str, i + 16), __GMOCK_VPTR_STR_IMPL_16(str, i + 32)

#undef EXPECT_CALL
#define __GMOCK_VPTR_EXPECT_CALL_0(obj, call) GMOCK_EXPECT_CALL_IMPL_(obj, call)
#define __GMOCK_VPTR_EXPECT_CALL_1(obj, call)                                              \
  ((obj).gmock_call<__GMOCK_VPTR_QNAME call>(                                              \
       &std::decay_t<decltype(obj)>::type::__GMOCK_VPTR_NAME call __GMOCK_VPTR_CALL call)) \
      .InternalExpectedAt(__FILE__, __LINE__, #obj, #call)
#define EXPECT_CALL(obj, call) __GMOCK_VPTR_CAT(__GMOCK_VPTR_EXPECT_CALL_, __GMOCK_VPTR_IBP(call))(obj, call)

#undef ON_CALL
#define __GMOCK_VPTR_ON_CALL_0(obj, call) GMOCK_ON_CALL_IMPL_(obj, call)
#define __GMOCK_VPTR_ON_CALL_1(obj, call)                                                  \
  ((obj).gmock_call<__GMOCK_VPTR_QNAME call>(                                              \
       &std::decay_t<decltype(obj)>::type::__GMOCK_VPTR_NAME call __GMOCK_VPTR_CALL call)) \
      .InternalDefaultActionSetAt(__FILE__, __LINE__, #obj, #call)
#define ON_CALL(obj, call) __GMOCK_VPTR_CAT(__GMOCK_VPTR_ON_CALL_, __GMOCK_VPTR_IBP(call))(obj, call)
