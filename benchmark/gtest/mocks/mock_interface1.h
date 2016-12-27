//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "gmock/gmock.h"
#include "interface1.h"

struct mock_interface1 : interface1 {
  MOCK_CONST_METHOD1(f1, bool(int));
};
