//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <fstream>
#include <stdexcept>
#include <string>

namespace testing {
inline namespace v1 {
namespace detail {

inline auto basename(const std::string &path) {
  return path.substr(path.find_last_of("/\\") + 1);
}

inline std::wstring read_file(const std::string &feature) {
  std::ifstream file{feature};
  if (!file.good()) {
    throw std::runtime_error("File \"" + feature + "\" not found!");
  }
  return {(std::istreambuf_iterator<char>(file)),
          std::istreambuf_iterator<char>()};
}

}  // detail
}  // v1
}  // testing
