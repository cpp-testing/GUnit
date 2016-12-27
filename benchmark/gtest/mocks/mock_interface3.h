//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "gmock/gmock.h"
#include "interface3.h"

struct mock_interface3 : interface3 {
  MOCK_METHOD3(f3, void(int, int, int));
  MOCK_METHOD0(f3_1, void());
  MOCK_METHOD0(f3_2, void());
  MOCK_METHOD0(f3_3, void());
  MOCK_METHOD0(f3_4, void());
  MOCK_METHOD0(f3_5, void());
  MOCK_METHOD0(f3_6, void());
  MOCK_METHOD0(f3_7, void());
  MOCK_METHOD0(f3_8, void());
  MOCK_METHOD0(f3_9, void());
  MOCK_METHOD0(f3_10, void());
};
