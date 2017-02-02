//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/Detail/TypeTraits.h"
#include <gtest/gtest.h>

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

TEST(TypeTraits, ShouldReturnUniqueTypeId) {
  struct a {};
  struct b {};
  EXPECT_TRUE(type_id<int>() == type_id<int>());
  EXPECT_TRUE(type_id<const int>() == type_id<int>());
  EXPECT_TRUE(type_id<const int>() == type_id<int const volatile>());
  EXPECT_FALSE(type_id<int>() == type_id<double>());
  EXPECT_FALSE(type_id<a>() == type_id<b>());
}

TEST(TypeTraits, ShouldReturnTrueIfTupleContaintsType) {
  EXPECT_FALSE((contains<int, std::tuple<>>::value));
  EXPECT_FALSE((contains<int, std::tuple<double, float, char>>::value));
  EXPECT_TRUE((contains<int, std::tuple<int>>::value));
  EXPECT_TRUE((contains<int, std::tuple<int, double>>::value));
  EXPECT_TRUE((contains<int, std::tuple<int, double, int>>::value));
  EXPECT_TRUE((contains<int, std::tuple<double, float, int>>::value));
}
}
}
}
