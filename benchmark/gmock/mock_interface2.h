#pragma once

#include "gmock/gmock.h"
#include "interface2.h"

struct mock_interface2 : interface2 {
  MOCK_METHOD0(f2_1, void());
  MOCK_METHOD0(f2_2, void());
  MOCK_METHOD0(f2_3, void());
  MOCK_METHOD0(f2_4, void());
};
