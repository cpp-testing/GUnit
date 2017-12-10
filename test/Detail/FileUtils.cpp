//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <gtest/gtest.h>

#include "GUnit/Detail/FileUtils.h"

namespace testing {
inline namespace v1 {
namespace detail {

TEST(FileUtils, ShouldReturnBaseName) {
  EXPECT_EQ(std::string{"file.hpp"}, basename("file.hpp"));
  EXPECT_EQ(std::string{"file.hpp"}, basename("/a/b/file.hpp"));
  EXPECT_EQ(std::string{"file.hpp"}, basename("/b/file.hpp"));
}

}  // detail
}  // v1
}  // testing
