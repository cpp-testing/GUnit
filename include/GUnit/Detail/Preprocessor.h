//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#define __GUNIT_COMMA() ,
#define __GUNIT_IGNORE(...)
#define __GUNIT_PRIMITIVE_CAT(arg, ...) arg##__VA_ARGS__
#define __GUNIT_CAT(arg, ...) __GUNIT_PRIMITIVE_CAT(arg, __VA_ARGS__)
#define __GUNIT_SIZE(...)                                                     \
  __GUNIT_CAT(__GUNIT_VARIADIC_SIZE(__VA_ARGS__, 11, 10, 9, 8, 7, 6, 5, 4, 3, \
                                    2, 1, ), )
#define __GUNIT_VARIADIC_SIZE(e0, e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, \
                              size, ...)                                   \
  size
#define __GUNIT_IBP_SPLIT(i, ...) \
  __GUNIT_PRIMITIVE_CAT(__GUNIT_IBP_SPLIT_, i)(__VA_ARGS__)
#define __GUNIT_IBP_SPLIT_0(a, ...) a
#define __GUNIT_IBP_SPLIT_1(a, ...) __VA_ARGS__
#define __GUNIT_IBP_IS_VARIADIC_C(...) 1
#define __GUNIT_IBP_IS_VARIADIC_R_1 1,
#define __GUNIT_IBP_IS_VARIADIC_R___GUNIT_IBP_IS_VARIADIC_C 0,
#define __GUNIT_IBP(...)                                       \
  __GUNIT_IBP_SPLIT(0, __GUNIT_CAT(__GUNIT_IBP_IS_VARIADIC_R_, \
                                   __GUNIT_IBP_IS_VARIADIC_C __VA_ARGS__))
#define __GUNIT_IS_EMPTY(...)                    \
  __GUNIT_IS_EMPTY_IIF(__GUNIT_IBP(__VA_ARGS__)) \
  (__GUNIT_IS_EMPTY_GEN_ZERO, __GUNIT_IS_EMPTY_PROCESS)(__VA_ARGS__)
#define __GUNIT_IS_EMPTY_PRIMITIVE_CAT(a, b) a##b
#define __GUNIT_IS_EMPTY_IIF(bit) \
  __GUNIT_IS_EMPTY_PRIMITIVE_CAT(__GUNIT_IS_EMPTY_IIF_, bit)
#define __GUNIT_IS_EMPTY_NON_FUNCTION_C(...) ()
#define __GUNIT_IS_EMPTY_GEN_ZERO(...) 0
#define __GUNIT_IS_EMPTY_IIF_0(t, b) b
#define __GUNIT_IS_EMPTY_IIF_1(t, b) t
#define __GUNIT_IS_EMPTY_PROCESS(...) \
  __GUNIT_IBP(__GUNIT_IS_EMPTY_NON_FUNCTION_C __VA_ARGS__())
#define __GUNIT_IIF(c) __GUNIT_PRIMITIVE_CAT(__GUNIT_IIF_, c)
#define __GUNIT_IIF_0(t, ...) __VA_ARGS__
#define __GUNIT_IIF_1(t, ...) t
#define __GUNIT_IF(c) __GUNIT_IIF(c)
