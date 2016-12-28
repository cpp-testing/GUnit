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
#include <stdexcept>

namespace testing
{
namespace detail
{

template <class T>
struct identity
{
    using type = T;
};

template <class T>
using identity_t = typename identity<T>::type;

template <class TDst, class TSrc>
inline TDst union_cast( TSrc src )
{
    union
    {
        TSrc src;
        TDst dst;
    } u;
    u.src = src;
    return u.dst;
}

template <char... Chrs>
struct string
{
    static auto c_str()
    {
        static constexpr char str[] = { Chrs..., 0 };
        return str;
    }
};

template <class T>
inline auto vptr_offset( T f )
{
    struct vptr
    {
        virtual int f0() { return 0; }
        virtual int f1() { return 1; }
        virtual int f2() { return 2; }
        virtual int f3() { return 3; }
        virtual int f4() { return 4; }
        virtual int f5() { return 5; }
        virtual int f6() { return 6; }
        virtual int f7() { return 7; }
        virtual int f8() { return 8; }
        virtual int f9() { return 9; }
        virtual int f10() { return 10; }
        virtual int f11() { return 11; }
        virtual int f12() { return 12; }
        virtual int f13() { return 13; }
        virtual int f14() { return 14; }
        virtual int f15() { return 15; }
        virtual int f16() { return 16; }
        virtual int f17() { return 17; }
        virtual int f18() { return 18; }
        virtual int f19() { return 19; }
        virtual int f20() { return 20; }
        virtual int f21() { return 21; }
        virtual int f22() { return 22; }
        virtual int f23() { return 23; }
        virtual int f24() { return 24; }
        virtual int f25() { return 25; }
        virtual int f26() { return 26; }
        virtual int f27() { return 27; }
        virtual int f28() { return 28; }
        virtual int f29() { return 29; }
        virtual int f30() { return 30; }
        virtual int f31() { return 31; }
        virtual int f32() { return 32; }
        virtual int f33() { return 33; }
        virtual int f34() { return 34; }
        virtual int f35() { return 35; }
        virtual int f36() { return 36; }
        virtual int f37() { return 37; }
        virtual int f38() { return 38; }
        virtual int f39() { return 39; }
        virtual int f40() { return 40; }
        virtual int f41() { return 41; }
        virtual int f42() { return 42; }
        virtual int f43() { return 43; }
        virtual int f44() { return 44; }
        virtual int f45() { return 45; }
        virtual int f46() { return 46; }
        virtual int f47() { return 47; }
        virtual int f48() { return 48; }
        virtual int f49() { return 49; }
        virtual int f50() { return 50; }
        virtual int f51() { return 51; }
        virtual int f52() { return 52; }
        virtual int f53() { return 53; }
        virtual int f54() { return 54; }
        virtual int f55() { return 55; }
        virtual int f56() { return 56; }
        virtual int f57() { return 57; }
        virtual int f58() { return 58; }
        virtual int f59() { return 59; }
        virtual int f60() { return 60; }
        virtual int f61() { return 61; }
        virtual int f62() { return 62; }
        virtual int f63() { return 63; }
        virtual int f64() { return 64; }
        virtual int f65() { return 65; }
        virtual int f66() { return 66; }
        virtual int f67() { return 67; }
        virtual int f68() { return 68; }
        virtual int f69() { return 69; }
        virtual int f70() { return 70; }
        virtual int f71() { return 71; }
        virtual int f72() { return 72; }
        virtual int f73() { return 73; }
        virtual int f74() { return 74; }
        virtual int f75() { return 75; }
        virtual int f76() { return 76; }
        virtual int f77() { return 77; }
        virtual int f78() { return 78; }
        virtual int f79() { return 79; }
        virtual int f80() { return 80; }
        virtual int f81() { return 81; }
        virtual int f82() { return 82; }
        virtual int f83() { return 83; }
        virtual int f84() { return 84; }
        virtual int f85() { return 85; }
        virtual int f86() { return 86; }
        virtual int f87() { return 87; }
        virtual int f88() { return 88; }
        virtual int f89() { return 89; }
        virtual int f90() { return 90; }
        virtual int f91() { return 91; }
        virtual int f92() { return 92; }
        virtual int f93() { return 93; }
        virtual int f94() { return 94; }
        virtual int f95() { return 95; }
        virtual int f96() { return 96; }
        virtual int f97() { return 97; }
        virtual int f98() { return 98; }
        virtual int f99() { return 99; }
        virtual int f100() { return 100; }
        virtual int f101() { return 101; }
        virtual int f102() { return 102; }
        virtual int f103() { return 103; }
        virtual int f104() { return 104; }
        virtual int f105() { return 105; }
        virtual int f106() { return 106; }
        virtual int f107() { return 107; }
        virtual int f108() { return 108; }
        virtual int f109() { return 109; }
        virtual int f110() { return 110; }
        virtual int f111() { return 111; }
        virtual int f112() { return 112; }
        virtual int f113() { return 113; }
        virtual int f114() { return 114; }
        virtual int f115() { return 115; }
        virtual int f116() { return 116; }
        virtual int f117() { return 117; }
        virtual int f118() { return 118; }
        virtual int f119() { return 119; }
        virtual int f120() { return 120; }
        virtual int f121() { return 121; }
        virtual int f122() { return 122; }
        virtual int f123() { return 123; }
        virtual int f124() { return 124; }
        virtual int f125() { return 125; }
        virtual int f126() { return 126; }
        virtual int f127() { return 127; }
        virtual ~vptr() = default;
    } _;
    int ( vptr::*ptr )() = (int ( vptr::* )())f;
    return ( _.*ptr )();
}

} // detail

template <class T>
class GMock
{
    static_assert( std::is_polymorphic<T>::value, "T has to be a polymorphic type" );
    static_assert( std::has_virtual_destructor<T>::value, "T has to have a virtual destructor" );

    unsigned char _[ sizeof( T ) ] = { 0 };
    void*         old_vptr         = nullptr;
    void ( *vptr[ 128 ] )()        = { not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented(), not_implemented(), not_implemented(), not_implemented(),
        not_implemented() };

    auto not_implemented() { return detail::union_cast<void ( * )()>( &GMock::unknown ); }

    void* unknown()
    {
        auto* ptr = [this] {
            fs[ __PRETTY_FUNCTION__ ] = std::make_unique<FunctionMocker<void*()>>();
            return static_cast<FunctionMocker<void*()>*>( fs[ __PRETTY_FUNCTION__ ].get() );
        }();
        ptr->SetOwnerAndName( this, __PRETTY_FUNCTION__ );
        return ptr->Invoke();
    }

    void* update_vptr( void* new_vptr )
    {
        void* vptr    = *(void**)this;
        *(void**)this = new_vptr;
        return vptr;
    }

    template <class TName, class R, class... TArgs>
    decltype( auto ) gmock_call_impl(
        int offset, const detail::identity_t<Matcher<TArgs>>&... args )
    {
        vptr[ offset ] =
            detail::union_cast<void ( * )()>( &GMock::template original_call<TName, R, TArgs...> );

        const auto it  = fs.find( TName::c_str() );
        auto*      ptr = [&it, this] {
            if ( it == fs.end() )
            {
                fs[ TName::c_str() ] = std::make_unique<FunctionMocker<R( TArgs... )>>();
                return static_cast<FunctionMocker<R( TArgs... )>*>( fs[ TName::c_str() ].get() );
            }
            return static_cast<FunctionMocker<R( TArgs... )>*>( it->second.get() );
        }();

        ptr->RegisterOwner( this );
        return ptr->With( args... );
    }

    template <class TName, class R, class... TArgs>
    R original_call( TArgs... args )
    {
        auto* f = static_cast<FunctionMocker<R( TArgs... )>*>( fs[ TName::c_str() ].get() );
        f->SetOwnerAndName( this, TName::c_str() );
        return f->Invoke( args... );
    }

public:
    using type = T;

    GMock() : old_vptr( update_vptr( vptr ) ) {}

    template <class TName, class R, class B, class... TArgs>
    decltype( auto ) gmock_call(
        R ( B::*f )( TArgs... ), const detail::identity_t<Matcher<TArgs>>&... args )
    {
        return gmock_call_impl<TName, R, TArgs...>( detail::vptr_offset( f ), args... );
    }

    template <class TName, class R, class B, class... TArgs>
    decltype( auto ) gmock_call(
        R ( B::*f )( TArgs... ) const, const typename detail::identity_t<Matcher<TArgs>>&... args )
    {
        return gmock_call_impl<TName, R, TArgs...>( detail::vptr_offset( f ), args... );
    }

    operator T*() { return reinterpret_cast<T*>( this ); }
    operator T&() { return reinterpret_cast<T&>( *this ); }

private:
    std::unordered_map<std::string, std::unique_ptr<internal::UntypedFunctionMockerBase>> fs;
};

} // testing

#define __GMOCK_VPTR_COMMA() ,
#define __GMOCK_VPTR_IGNORE( ... )
#define __GMOCK_VPTR_NAME( ... ) __VA_ARGS__ __GMOCK_VPTR_IGNORE
#define __GMOCK_VPTR_QNAME( ... ) \
    ::testing::detail::string<__GMOCK_VPTR_STR_IMPL_32( #__VA_ARGS__, 0 ), 0> __GMOCK_VPTR_IGNORE
#define __GMOCK_VPTR_INTERNAL( ... )                      \
    __GMOCK_VPTR_IF( __BOOST_DI_IS_EMPTY( __VA_ARGS__ ) ) \
    ( __GMOCK_VPTR_IGNORE, __GMOCK_VPTR_COMMA )() __VA_ARGS__
#define __GMOCK_VPTR_CALL( ... ) __GMOCK_VPTR_INTERNAL
#define __GMOCK_VPTR_PRIMITIVE_CAT( arg, ... ) arg##__VA_ARGS__
#define __GMOCK_VPTR_CAT( arg, ... ) __GMOCK_VPTR_PRIMITIVE_CAT( arg, __VA_ARGS__ )
#define __GMOCK_VPTR_IBP_SPLIT( i, ... ) \
    __GMOCK_VPTR_PRIMITIVE_CAT( __GMOCK_VPTR_IBP_SPLIT_, i )( __VA_ARGS__ )
#define __GMOCK_VPTR_IBP_SPLIT_0( a, ... ) a
#define __GMOCK_VPTR_IBP_SPLIT_1( a, ... ) __VA_ARGS__
#define __GMOCK_VPTR_IBP_IS_VARIADIC_C( ... ) 1
#define __GMOCK_VPTR_IBP_IS_VARIADIC_R_1 1,
#define __GMOCK_VPTR_IBP_IS_VARIADIC_R___GMOCK_VPTR_IBP_IS_VARIADIC_C 0,
#define __GMOCK_VPTR_IBP( ... )                                                   \
    __GMOCK_VPTR_IBP_SPLIT( 0, __GMOCK_VPTR_CAT( __GMOCK_VPTR_IBP_IS_VARIADIC_R_, \
                                   __GMOCK_VPTR_IBP_IS_VARIADIC_C __VA_ARGS__ ) )
#define __BOOST_DI_IS_EMPTY( ... )                               \
    __GMOCK_VPTR_IS_EMPTY_IIF( __GMOCK_VPTR_IBP( __VA_ARGS__ ) ) \
    ( __GMOCK_VPTR_IS_EMPTY_GEN_ZERO, __GMOCK_VPTR_IS_EMPTY_PROCESS )( __VA_ARGS__ )
#define __GMOCK_VPTR_IS_EMPTY_PRIMITIVE_CAT( a, b ) a##b
#define __GMOCK_VPTR_IS_EMPTY_IIF( bit ) \
    __GMOCK_VPTR_IS_EMPTY_PRIMITIVE_CAT( __GMOCK_VPTR_IS_EMPTY_IIF_, bit )
#define __GMOCK_VPTR_IS_EMPTY_NON_FUNCTION_C( ... ) ()
#define __GMOCK_VPTR_IS_EMPTY_GEN_ZERO( ... ) 0
#define __GMOCK_VPTR_IS_EMPTY_IIF_0( t, b ) b
#define __GMOCK_VPTR_IS_EMPTY_IIF_1( t, b ) t
#define __GMOCK_VPTR_IS_EMPTY_PROCESS( ... ) \
    __GMOCK_VPTR_IBP( __GMOCK_VPTR_IS_EMPTY_NON_FUNCTION_C __VA_ARGS__() )
#define __GMOCK_VPTR_IIF( c ) __GMOCK_VPTR_PRIMITIVE_CAT( __GMOCK_VPTR_IIF_, c )
#define __GMOCK_VPTR_IIF_0( t, ... ) __VA_ARGS__
#define __GMOCK_VPTR_IIF_1( t, ... ) t
#define __GMOCK_VPTR_IF( c ) __GMOCK_VPTR_IIF( c )
#define __GMOCK_VPTR_STR_IMPL_1( str, i ) ( sizeof( str ) > ( i ) ? str[ ( i ) ] : 0 )
#define __GMOCK_VPTR_STR_IMPL_4( str, i )                                           \
    __GMOCK_VPTR_STR_IMPL_1( str, i + 0 )                                           \
    , __GMOCK_VPTR_STR_IMPL_1( str, i + 1 ), __GMOCK_VPTR_STR_IMPL_1( str, i + 2 ), \
        __GMOCK_VPTR_STR_IMPL_1( str, i + 3 )
#define __GMOCK_VPTR_STR_IMPL_16( str, i )                                          \
    __GMOCK_VPTR_STR_IMPL_4( str, i + 0 )                                           \
    , __GMOCK_VPTR_STR_IMPL_4( str, i + 4 ), __GMOCK_VPTR_STR_IMPL_4( str, i + 8 ), \
        __GMOCK_VPTR_STR_IMPL_4( str, i + 12 )
#define __GMOCK_VPTR_STR_IMPL_32( str, i ) \
    __GMOCK_VPTR_STR_IMPL_16( str, i + 0 ) \
    , __GMOCK_VPTR_STR_IMPL_16( str, i + 16 ), __GMOCK_VPTR_STR_IMPL_16( str, i + 32 )

#undef EXPECT_CALL
#define __GMOCK_VPTR_EXPECT_CALL_0( obj, call ) GMOCK_EXPECT_CALL_IMPL_( obj, call )
#define __GMOCK_VPTR_EXPECT_CALL_1( obj, call )                                                   \
    ( ( obj ).gmock_call<__GMOCK_VPTR_QNAME call>(                                                \
          &std::decay_t<decltype( obj )>::type::__GMOCK_VPTR_NAME call __GMOCK_VPTR_CALL call ) ) \
        .InternalExpectedAt( __FILE__, __LINE__, #obj, #call )
#define EXPECT_CALL( obj, call ) \
    __GMOCK_VPTR_CAT( __GMOCK_VPTR_EXPECT_CALL_, __GMOCK_VPTR_IBP( call ) )( obj, call )

#undef ON_CALL
#define __GMOCK_VPTR_ON_CALL_0( obj, call ) GMOCK_ON_CALL_IMPL_( obj, call )
#define __GMOCK_VPTR_ON_CALL_1( obj, call )                                                       \
    ( ( obj ).gmock_call<__GMOCK_VPTR_QNAME call>(                                                \
          &std::decay_t<decltype( obj )>::type::__GMOCK_VPTR_NAME call __GMOCK_VPTR_CALL call ) ) \
        .InternalDefaultActionSetAt( __FILE__, __LINE__, #obj, #call )
#define ON_CALL( obj, call ) \
    __GMOCK_VPTR_CAT( __GMOCK_VPTR_ON_CALL_, __GMOCK_VPTR_IBP( call ) )( obj, call )
