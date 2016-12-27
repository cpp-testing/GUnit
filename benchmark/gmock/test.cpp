#include "example.h"
#include "gmock/gmock.h"
#include "gmock/mock_interface1.h"
#include "gmock/mock_interface2.h"
#include "gmock/mock_interface3.h"

TEST(benchmark, test1) {
  using namespace testing;
  mock_interface1 m1;
  mock_interface2 m2;
  mock_interface3 m3;

  EXPECT_CALL(m1, f1(42)).WillOnce(Return(true));
  EXPECT_CALL(m2, f2_1()).Times(1);
  EXPECT_CALL(m3, f3(0, 1, 2)).Times(1);

  example e{m1, m2, m3};

  e.test();
}

TEST(benchmark, test2) {
  using namespace testing;
  mock_interface1 m1;
  mock_interface2 m2;
  mock_interface3 m3;

  EXPECT_CALL(m1, f1(42)).WillOnce(Return(false));
  EXPECT_CALL(m2, f2_2()).Times(1);
  EXPECT_CALL(m3, f3(0, 1, 2)).Times(1);

  example e{m1, m2, m3};

  e.test();
}