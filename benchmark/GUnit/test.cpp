//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <GUnit.h>
#include <memory>
#include "example.h"

class BenchmarkTest : public testing::GTest<example> {
 public:
  void SetUp() override { std::tie(sut, mocks) = testing::make<SUT, testing::NaggyGMock>(); }
};

TEST_F(BenchmarkTest, ShouldCallF1WhenF1ReturnsTrue) {
  using namespace testing;

  EXPECT_CALL(mock<interface1>(), (f1)(42)).WillOnce(Return(true));
  EXPECT_CALL(mock<interface2>(), (f2_1)()).Times(1);
  EXPECT_CALL(mock<interface3>(), (f3)(0, 1, 2)).Times(1);

  sut->test();
}

TEST_F(BenchmarkTest, ShouldCallF2WhenF1ReturnsFalse) {
  using namespace testing;

  EXPECT_CALL(mock<interface1>(), (f1)(42)).WillOnce(Return(false));
  EXPECT_CALL(mock<interface2>(), (f2_2)()).Times(1);
  EXPECT_CALL(mock<interface3>(), (f3)(0, 1, 2)).Times(1);

  sut->test();
}
