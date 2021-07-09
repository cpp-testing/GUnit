//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "GUnit/GAssert.h"
#include "GUnit/GMake.h"
#include "GUnit/GMock.h"
#include "GUnit/GTest-Lite.h"
#include "GUnit/GTest.h"

#if __has_include(<json.hpp>) and __has_include(<gherkin.hpp>)
#include "GUnit/GSteps.h"
#endif
