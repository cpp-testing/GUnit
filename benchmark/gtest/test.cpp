//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <gtest/gtest.h>
#include <memory>
#include "example.h"
#include "gtest/mocks/mock_interface1.h"
#include "gtest/mocks/mock_interface2.h"
#include "gtest/mocks/mock_interface3.h"

class BenchmarkTest : public testing::Test {
 public:
  void SetUp() override { sut = std::make_unique<example>(m1, m2, m3); }

  mock_interface1 m1;
  mock_interface2 m2;
  mock_interface3 m3;
  std::unique_ptr<example> sut;
};

TEST_F(BenchmarkTest, ShouldCallF1WhenF1ReturnsTrue) {
  using namespace testing;

  EXPECT_CALL(m1, f1(42)).WillOnce(Return(true));
  EXPECT_CALL(m2, f2_1()).Times(1);
  EXPECT_CALL(m3, f3(0, 1, 2)).Times(1);

  sut->test();
}

TEST_F(BenchmarkTest, ShouldCallF2WhenF1ReturnsFalse) {
  using namespace testing;

  EXPECT_CALL(m1, f1(42)).WillOnce(Return(false));
  EXPECT_CALL(m2, f2_2()).Times(1);
  EXPECT_CALL(m3, f3(0, 1, 2)).Times(1);

  sut->test();
}
