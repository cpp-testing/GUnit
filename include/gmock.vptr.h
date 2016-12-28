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

#if !defined( VPTR_SIZE )
#define VPTR_SIZE 16
#endif

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
        virtual ~vptr() = default;
    } _;
    int ( vptr::*ptr )() = (int ( vptr::* )())f;
    const auto   offset  = ( _.*ptr )();

    if ( offset > VPTR_SIZE )
    {
        throw std::runtime_error( "Too many virtual functions (VPTR_SIZE)! Consider using a Single "
                                  "Responsibility Principle" );
    }

    return offset;
}

} // detail

template <class T>
class GMock
{
    static_assert( std::is_polymorphic<T>::value, "T has to be a polymorphic type" );
    static_assert( std::has_virtual_destructor<T>::value, "T has to have a virtual destructor" );

    unsigned char _[ sizeof( T ) ] = { 0 };
    void*         old_vptr         = nullptr;
    void ( *vptr[ VPTR_SIZE ] )()  = { detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ),
        detail::union_cast<void ( * )()>( &GMock::unknown ) };

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
