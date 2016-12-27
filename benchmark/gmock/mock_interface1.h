#pragma once

#include "gmock/gmock.h"
#include "interface1.h"

struct mock_interface1 : interface1 {
  MOCK_CONST_METHOD1(f1, bool(int));
};
