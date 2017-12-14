//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <string>
#include <vector>

#include "GUnit/Detail/StringUtils.h"

namespace testing {
inline namespace v1 {
namespace detail {

template <class T>
inline constexpr auto args_size(T str) {
  auto args = 0;
  for (auto i = 0u; i < str.size(); ++i) {
    if (str[i] == '{') {
      ++args;
    }
  }
  return args;
}

inline auto remove_comments(const std::string& str) {
  const auto comment = str.find("#");
  if (comment != std::string::npos) {
    return str.substr(0, comment - 1);
  }
  return str;
}

template <class T>
inline auto matches(T pattern, const std::string& txt) {
  std::string str{remove_comments(txt)};
  std::vector<std::string> matches{};
  auto pi = 0u, si = 0u;

  const auto matcher = [&](char b, char e, char c = 0) {
    const auto match = si;
    while (str[si] && str[si] != b && str[si] != c) ++si;
    matches.emplace_back(str.substr(match, si - match));
    while (pattern[pi] && pattern[pi] != e) ++pi;
    pi++;
  };

  while (pi < pattern.size() && si < str.size()) {
    if (pattern[pi] == '\'' && str[si] == '\'' && pattern[pi + 1] == '{') {
      ++si;
      matcher('\'', '}');
    } else if (pattern[pi] == '{') {
      matcher(' ', '}', ',');
    } else if (pattern[pi] != str[si]) {
      return std::vector<std::string>{};
    }
    ++pi, ++si;
  }

  if (si < str.size() || pi < pattern.size()) {
    return std::vector<std::string>{};
  }

  return matches;
}

template <class T>
inline auto match(T pattern, const std::string& str) {
  return not matches(pattern, str).empty() ||
         std::string{pattern.c_str()} == remove_comments(str);
}

inline bool PatternMatchesString(const char* pattern, const char* str) {
  switch (*pattern) {
    case '\0':
    case ':':  // Either ':' or '\0' marks the end of the pattern.
      return *str == '\0';
    case '?':  // Matches any single character.
      return *str != '\0' && PatternMatchesString(pattern + 1, str + 1);
    case '*':  // Matches any string (possibly empty) of characters.
      return (*str != '\0' && PatternMatchesString(pattern, str + 1)) ||
             PatternMatchesString(pattern + 1, str);
    default:  // Non-special character.  Matches itself.
      return *pattern == *str && PatternMatchesString(pattern + 1, str + 1);
  }
}

inline bool MatchesFilter(const std::string& name, const char* filter) {
  const char* cur_pattern = filter;
  for (;;) {
    if (PatternMatchesString(cur_pattern, name.c_str())) {
      return true;
    }

    // Finds the next pattern in the filter.
    cur_pattern = strchr(cur_pattern, ':');

    // Returns if no more pattern can be found.
    if (cur_pattern == nullptr) {
      return false;
    }

    // Skips the pattern separater (the ':' character).
    cur_pattern++;
  }
}

inline bool FilterMatchesShould(const std::string& name,
                                const std::string& should) {
  // Split --gtest_filter at '-', if there is one, to separate into
  // positive filter and negative filter portions
  const char* const p = should.c_str();
  const char* const dash = strchr(p, '-');
  std::string positive;
  std::string negative;
  if (dash == nullptr) {
    positive = should.c_str();  // Whole string is a positive filter
    negative = "";
  } else {
    positive = std::string(p, dash);   // Everything up to the dash
    negative = std::string(dash + 1);  // Everything after the dash
    if (positive.empty()) {
      // Treat '-test1' as the same as '*-test1'
      positive = "*";
    }
  }

  return MatchesFilter(name, positive.c_str()) &&
         !MatchesFilter(name, negative.c_str());
}

}  // detail
}  // v1
}  // testing
