//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

namespace testing {
inline namespace v1 {
namespace detail {

// This function is lifted from GTest's internals.
// Returns true iff colors should be used in the output.
inline bool ShouldUseColor(bool stdout_is_tty) {
  using internal::String;

  const char* const gtest_color = GTEST_FLAG(color).c_str();

  if (String::CaseInsensitiveCStringEquals(gtest_color, "auto")) {
    const char* const term = internal::posix::GetEnv("TERM");
    const bool term_supports_color =
        String::CStringEquals(term, "xterm") ||
        String::CStringEquals(term, "xterm-color") ||
        String::CStringEquals(term, "xterm-256color") ||
        String::CStringEquals(term, "screen") ||
        String::CStringEquals(term, "screen-256color") ||
        String::CStringEquals(term, "tmux") ||
        String::CStringEquals(term, "tmux-256color") ||
        String::CStringEquals(term, "rxvt-unicode") ||
        String::CStringEquals(term, "rxvt-unicode-256color") ||
        String::CStringEquals(term, "linux") ||
        String::CStringEquals(term, "cygwin");
    return stdout_is_tty && term_supports_color;
  }

  return String::CaseInsensitiveCStringEquals(gtest_color, "yes") ||
         String::CaseInsensitiveCStringEquals(gtest_color, "true") ||
         String::CaseInsensitiveCStringEquals(gtest_color, "t") ||
         String::CStringEquals(gtest_color, "1");
  // We take "yes", "true", "t", and "1" as meaning "yes".  If the value is
  // neither one of these nor "auto", we treat it as "no"
  // to be conservative.
}

}  // detail
}  // v1
}  // testing
