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
#include <unistd.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <memory>
#include <set>
#include <sstream>

#if defined(__APPLE__)
#include <libproc.h>
#elif defined(__linux__)
extern const char *__progname_full;
#endif

#if !defined(GUNIT_SHOW_STACK_SIZE)
#define GUNIT_SHOW_STACK_SIZE 3
#endif

namespace testing {
inline namespace v1 {
namespace detail {

template <class TDst, class TSrc>
inline TDst union_cast(TSrc src) {
  union {
    TSrc src;
    TDst dst;
  } u;
  u.src = src;
  return u.dst;
}

template <char... Chrs>
struct string {
  static auto c_str() {
    static constexpr char str[] = {Chrs..., 0};
    return str;
  }

  template <char... Chrs_>
  constexpr auto operator+(string<Chrs_...>) {
    return string<Chrs..., Chrs_...>{};
  }
};

namespace operators {

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif

template <class T, T... Chrs>
constexpr auto operator""_gtest_string() {
  return string<Chrs...>{};
}

}  // operators

template <class, std::size_t N, std::size_t... Ns>
auto get_type_name_impl(const char *ptr, std::index_sequence<Ns...>) {
  static const char str[] = {ptr[N + Ns]..., 0};
  return str;
}

template <class T>
const char *get_type_name() {
#if defined(__clang__)
  return get_type_name_impl<T, 54>(__PRETTY_FUNCTION__, std::make_index_sequence<sizeof(__PRETTY_FUNCTION__) - 54 - 2>{});
#elif defined(__GNUC__)
  return get_type_name_impl<T, 59>(__PRETTY_FUNCTION__, std::make_index_sequence<sizeof(__PRETTY_FUNCTION__) - 59 - 2>{});
#endif
}

inline auto basename(const std::string &path) { return path.substr(path.find_last_of("/\\") + 1); }

inline void trim(std::string &txt) {
  txt.erase(0, txt.find_first_not_of(" \n\r\t"));
  txt.erase(txt.find_last_not_of(" \n\r\t") + 1);
}

inline std::string demangle(const std::string &mangled) {
  const auto demangled = abi::__cxa_demangle(mangled.c_str(), 0, 0, 0);
  if (demangled) {
    std::shared_ptr<char> free{demangled, std::free};
    return demangled;
  }
  return {};
}

inline std::string call_stack(const std::string &newline, int stack_size = GUNIT_SHOW_STACK_SIZE) {
  static constexpr auto MAX_CALL_STACK_SIZE = 64;
  void *bt[MAX_CALL_STACK_SIZE];
  const auto frames = backtrace(bt, sizeof(bt) / sizeof(bt[0]));
  const auto symbols = backtrace_symbols(bt, frames);
  std::shared_ptr<char *> free{symbols, std::free};
  std::stringstream result;

  for (auto i = 1; i < (frames > stack_size ? stack_size : frames); ++i) {
    const auto symbol = std::string{symbols[i]};

    const auto name_begin = symbol.find("(");
    const auto name_end = symbol.find("+");
    const auto address_begin = symbol.find("[");
    const auto address_end = symbol.find("]");

    if (name_begin != std::string::npos && name_end != std::string::npos && address_begin != std::string::npos &&
        address_end != std::string::npos) {
      result << demangle(symbol.substr(name_begin + 1, name_end - name_begin - 1)) << " "
             << symbol.substr(address_begin, address_end - address_begin + 1);
    } else {
      result << symbol;
    }
    result << newline;
  }
  return result.str();
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

template <class TParser>
inline auto symbols(const std::string &symbol) {
  std::set<typename TParser::type> result;
  std::stringstream cmd;
  cmd << "nm -gpP " << progname();
  auto fp = popen(cmd.str().c_str(), "r");
  if (fp) {
    char buf[8192] = {};
    while (fgets(buf, sizeof(buf), fp)) {
      if (!strncmp(buf, symbol.c_str(), symbol.length())) {
        auto i = symbol.length() + 1;
        while (buf[i] != ' ' && buf[i]) ++i;
        buf[i] = 0;
        result.emplace(TParser::parse(demangle(buf)));
      }
    }
  }
  pclose(fp);
  return result;
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
