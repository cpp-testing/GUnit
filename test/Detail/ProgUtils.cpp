//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <gtest/gtest.h>
#include <gmock/gmock.h> // MatchesRegex

#include "GUnit/Detail/ProgUtils.h"

namespace testing {
inline namespace v1 {
namespace detail {

TEST(ProgUtils, ShouldReturnDemangledName) {
  EXPECT_EQ(std::string{}, demangle(""));
  EXPECT_EQ(std::string{}, demangle("!!!"));
  EXPECT_EQ(std::string{"std::ignore"}, demangle("_ZStL6ignore"));
  EXPECT_EQ(std::string{"std::vector<double, std::allocator<double> >::~vector()"}, demangle("_ZNSt6vectorIdSaIdEED1Ev"));
}

#if !defined(__APPLE__)
TEST(ProgUtils, ShouldReturnProgFullPath) { EXPECT_EQ(internal::GetArgvs()[0], progname()); }
#endif

TEST(ProgUtils, ShouldReturnCallStack) {
  EXPECT_EQ(std::string{}, call_stack("\n", 0, 0));
  EXPECT_EQ(std::string{}, call_stack("\n", 1, 0));
  EXPECT_THAT(call_stack("\n", 1, 1), testing::MatchesRegex(".*ProgUtils_ShouldReturnCallStack_Test.*"));
  EXPECT_THAT(call_stack("\n", 1, 2), testing::MatchesRegex(".*ProgUtils_ShouldReturnCallStack_Test.*"));
}

} // detail
} // v1
} // testing
