//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "GUnit/GTest-Lite.h"
#include <cstdlib>

// clang-format off

int main() {

  "should compile"_test = [] {
      std::exit(0);
  };

  "should not run"_test_disabled = [] {
      std::exit(1);
  };

}

// clang-format on
