//
// Copyright (c) 2016 Krzysztof Jusiak (krzysztof at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GMock.h"
#include "example.h"

TEST(benchmark, test1) {
  using namespace testing;
  GMock<interface1> m1;
  GMock<interface2> m2;
  GMock<interface3> m3;

  EXPECT_CALL(m1, (f1)(42)).WillOnce(Return(true));
  EXPECT_CALL(m2, (f2_1)()).Times(1);
  EXPECT_CALL(m3, (f3)(0, 1, 2)).Times(1);

  example e{m1, m2, m3};

  e.test();
}

TEST(benchmark, test2) {
  using namespace testing;
  GMock<interface1> m1;
  GMock<interface2> m2;
  GMock<interface3> m3;

  EXPECT_CALL(m1, (f1)(42)).WillOnce(Return(false));
  EXPECT_CALL(m2, (f2_2)()).Times(1);
  EXPECT_CALL(m3, (f3)(0, 1, 2)).Times(1);

  example e{m1, m2, m3};

  e.test();
}
