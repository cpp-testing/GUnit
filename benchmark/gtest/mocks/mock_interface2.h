//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "gmock/gmock.h"
#include "interface2.h"

struct mock_interface2 : interface2 {
  MOCK_METHOD0(f2_1, void());
  MOCK_METHOD0(f2_2, void());
  MOCK_METHOD0(f2_3, void());
  MOCK_METHOD0(f2_4, void());
};
