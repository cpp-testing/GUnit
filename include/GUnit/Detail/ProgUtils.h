//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <cxxabi.h>
#include <execinfo.h>
#include <memory>
#include <string>
#include "gtest/gtest.h"

#if defined(__APPLE__)
#include <libproc.h>
#elif defined(__linux__)
extern const char *__progname_full;
#endif

#if !defined(GUNIT_SHOW_STACK_SIZE)
#define GUNIT_SHOW_STACK_SIZE 1
#endif

namespace testing {
inline namespace v1 {
namespace detail {

inline std::string demangle(const std::string &mangled) {
  const auto demangled = abi::__cxa_demangle(mangled.c_str(), 0, 0, 0);
  if (demangled) {
    std::shared_ptr<char> free{demangled, std::free};
    return demangled;
  }
  return {};
}

inline auto &progname() {
#if defined(__linux__)
  static auto self = __progname_full;
#elif defined(__APPLE__)
  static char self[PROC_PIDPATHINFO_MAXSIZE] = {};
  proc_pidpath(getpid(), self, sizeof(self));
#endif
  return self;
}

inline std::string call_stack(const std::string &newline, int stack_begin = 1,
                              int stack_size = GUNIT_SHOW_STACK_SIZE) {
  static constexpr auto MAX_CALL_STACK_SIZE = 64;
  void *bt[MAX_CALL_STACK_SIZE];
  const auto frames = backtrace(bt, sizeof(bt) / sizeof(bt[0]));
  const auto symbols = backtrace_symbols(bt, frames);
  std::shared_ptr<char *> free{symbols, std::free};
  std::stringstream result;
  stack_size += stack_begin;

  for (auto i = stack_begin; i < (frames > stack_size ? stack_size : frames);
       ++i) {
    const auto symbol = std::string{symbols[i]};
    const auto name_begin = symbol.find("(");
    const auto name_end = symbol.find("+");
    const auto address_begin = symbol.find("[");
    const auto address_end = symbol.find("]");

    if (i > stack_begin) {
      result << newline;
    }
    if (name_begin != std::string::npos && name_end != std::string::npos &&
        address_begin != std::string::npos &&
        address_end != std::string::npos) {
      result << demangle(
                    symbol.substr(name_begin + 1, name_end - name_begin - 1))
             << " "
             << symbol.substr(address_begin, address_end - address_begin + 1);
    } else {
      result << symbol;
    }
  }
  return result.str();
}

inline std::pair<std::string, int> addr2line(void *addr) {
  std::stringstream cmd;
  cmd << "addr2line -Cpe " << progname() << " " << addr;

  std::string data;
  auto fp = popen(cmd.str().c_str(), "r");
  if (fp) {
    char buf[64] = {};
    while (fgets(buf, sizeof(buf), fp)) {
      data += buf;
    }
  } else {
    return {"", 0};
  }
  pclose(fp);

  data.erase(std::remove(data.begin(), data.end(), '\n'), data.end());
  const auto space = data.find(" ");
  std::string res2 = data.substr(0, space);
  const auto colon = res2.find(":");
  return {res2.substr(0, colon), std::atoi(res2.substr(colon + 1).c_str())};
}

}  // detail
}  // v1
}  // testing
