//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/Detail/TypeTraits.h"
#include <gtest/gtest.h>

struct a {};

namespace testing {
inline namespace v1 {
namespace detail {

TEST(TypeTraits, ShouldReturnTrueIfIsComplete) {
  struct a {};
  class b;
  EXPECT_TRUE(is_complete<int>::value);
  EXPECT_TRUE(is_complete<a>::value);
  EXPECT_FALSE(is_complete<class B>::value);
  EXPECT_FALSE(is_complete<b>::value);
}

TEST(TypeTraits, ShouldReturnTrueIfIsSharedPtr) {
  EXPECT_FALSE(is_shared_ptr<int>::value);
  EXPECT_TRUE(is_shared_ptr<std::shared_ptr<int>>::value);
  EXPECT_TRUE(is_shared_ptr<const std::shared_ptr<int>>::value);
  EXPECT_TRUE(is_shared_ptr<const volatile std::shared_ptr<int>>::value);
}

TEST(TypeTraits, ShouldReturnTrueIfIsBaseOf) {
  struct b {};
  struct a : b {};
  EXPECT_TRUE((is_complete_base_of<b, a>::value));
  EXPECT_FALSE((is_complete_base_of<int, double>::value));
  EXPECT_FALSE((is_complete_base_of<class d, class e>::value));
}

TEST(TypeTraits, ShouldReturnTrueIfIsCallable) {
  struct a {
    void operator()(int) {}
  };

  struct b {
    void operator()() const {}
  };

  EXPECT_TRUE(is_callable<a(int)>::value);
  EXPECT_FALSE(is_callable<a()>::value);
  EXPECT_TRUE(is_callable<b()>::value);
  EXPECT_FALSE(is_callable<b(int, double)>::value);
}

TEST(TypeTraits, ShouldReturnUniqueTypeId) {
  struct a {};
  struct b {};
  EXPECT_TRUE(type_id<int>() == type_id<int>());
  EXPECT_TRUE(type_id<const int>() == type_id<int>());
  EXPECT_TRUE(type_id<const int>() == type_id<int const volatile>());
  EXPECT_FALSE(type_id<int>() == type_id<double>());
  EXPECT_FALSE(type_id<a>() == type_id<b>());
}

void f1() {}
int f2(int) { return {}; }
int f3(int, const double&) { return {}; }

struct c1 {
  void f1() {}
  int f2(int) { return {}; }
  int f3(int, const double&) { return {}; }
};

struct c2 {
  void f1() const {}
  int f2(int) const { return {}; }
  int f3(int, const double&) const { return {}; }
};

TEST(TypeTraits, ShouldReturnFunctionArguments) {
  EXPECT_TRUE(
      (std::is_same<
          void, typename function_traits<decltype(&f1)>::result_type>::value));
  EXPECT_TRUE(
      (std::is_same<type_list<>,
                    typename function_traits<decltype(&f1)>::args>::value));
  EXPECT_TRUE(
      (std::is_same<
          int, typename function_traits<decltype(&f2)>::result_type>::value));
  EXPECT_TRUE(
      (std::is_same<type_list<int>,
                    typename function_traits<decltype(&f2)>::args>::value));
  EXPECT_TRUE(
      (std::is_same<
          int, typename function_traits<decltype(&f3)>::result_type>::value));
  EXPECT_TRUE(
      (std::is_same<type_list<int, const double&>,
                    typename function_traits<decltype(&f3)>::args>::value));
  EXPECT_TRUE((std::is_same<void, typename function_traits<decltype(
                                      &c1::f1)>::result_type>::value));
  EXPECT_TRUE(
      (std::is_same<type_list<>,
                    typename function_traits<decltype(&c1::f1)>::args>::value));
  EXPECT_TRUE((std::is_same<int, typename function_traits<decltype(
                                     &c1::f2)>::result_type>::value));
  EXPECT_TRUE(
      (std::is_same<type_list<int>,
                    typename function_traits<decltype(&c1::f2)>::args>::value));
  EXPECT_TRUE((std::is_same<int, typename function_traits<decltype(
                                     &c1::f3)>::result_type>::value));
  EXPECT_TRUE(
      (std::is_same<type_list<int, const double&>,
                    typename function_traits<decltype(&c1::f3)>::args>::value));
  EXPECT_TRUE((std::is_same<void, typename function_traits<decltype(
                                      &c2::f1)>::result_type>::value));
  EXPECT_TRUE(
      (std::is_same<type_list<>,
                    typename function_traits<decltype(&c2::f1)>::args>::value));
  EXPECT_TRUE((std::is_same<int, typename function_traits<decltype(
                                     &c2::f2)>::result_type>::value));
  EXPECT_TRUE(
      (std::is_same<type_list<int>,
                    typename function_traits<decltype(&c2::f2)>::args>::value));
  EXPECT_TRUE((std::is_same<int, typename function_traits<decltype(
                                     &c2::f3)>::result_type>::value));
  EXPECT_TRUE(
      (std::is_same<type_list<int, const double&>,
                    typename function_traits<decltype(&c2::f3)>::args>::value));
}

struct n {};

TEST(TypeTraits, ShouldGetTypeName) {
  EXPECT_STREQ("int", get_type_name<int>());
  EXPECT_STREQ("const double", get_type_name<const double>());
  EXPECT_STREQ("a", get_type_name<a>());
  EXPECT_STREQ("testing::v1::detail::n", get_type_name<n>());
}
}  // detail
}  // v1
}  // testing
