//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "interface1.h"
#include "interface2.h"
#include "interface3.h"

class example {
 public:
  example(const interface1& i1, interface2& i2, interface3& i3)
      : i1(i1), i2(i2), i3(i3) {}

  void test() {
    if (i1.f1(42)) {
      i2.f2_1();
    } else {
      i2.f2_2();
    }
    i3.f3(0, 1, 2);
  }

 private:
  const interface1& i1;
  interface2& i2;
  interface3& i3;
};
