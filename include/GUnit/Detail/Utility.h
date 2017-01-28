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
#include <cstdio>
#include <cstring>
#include <sstream>
#include <vector>

namespace testing {
inline namespace v1 {
namespace detail {

template <char... Chrs>
struct string {
  static auto c_str() {
    static constexpr char str[] = {Chrs..., 0};
    return str;
  }
};

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif

template <class T, T... Chrs>
auto operator""_s() {
  return string<Chrs...>{};
}

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

template <class TDst, class TSrc>
inline TDst union_cast(TSrc src) {
  union {
    TSrc src;
    TDst dst;
  } u;
  u.src = src;
  return u.dst;
}

inline std::string demangle(const std::string &mangled) {
  const auto demangled = abi::__cxa_demangle(mangled.c_str(), 0, 0, 0);
  if (demangled) {
    std::shared_ptr<char> free{demangled, std::free};
    return demangled;
  }
  return {};
}

inline std::string call_stack() {
  static constexpr auto GUNIT_SHOW_STACK_SIZE = 3;
  static constexpr auto CALL_STACK_SIZE = 64;
  void *bt[CALL_STACK_SIZE];
  const auto frames = backtrace(bt, sizeof(bt) / sizeof(bt[0]));
  const auto symbols = backtrace_symbols(bt, frames);
  std::shared_ptr<char *> free{symbols, std::free};
  std::stringstream result;

  for (auto i = 1; i < (frames > GUNIT_SHOW_STACK_SIZE ? GUNIT_SHOW_STACK_SIZE : frames); ++i) {
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
    result << "\n\t\t   ";
  }
  return result.str();
}

inline auto get_self_name() {
  std::string res;

  res.resize(16);
  int rlin_size = ::readlink("/proc/self/exe", &res[0], res.size() - 1);
  while (rlin_size == static_cast<int>(res.size() - 1)) {
    res.resize(res.size() * 4);
    rlin_size = ::readlink("/proc/self/exe", &res[0], res.size() - 1);
  }
  if (rlin_size == -1) {
    return std::string{};
  }
  res.resize(rlin_size);
  return res;
}

inline auto nm() {
  std::vector<std::string> result;
  std::stringstream cmd;
  cmd << "nm -C " << get_self_name();
  auto fp = popen(cmd.str().c_str(), "r");
  if (fp) {
    char buf[16536];
    while (fgets(buf, sizeof(buf), fp)) {
      if (!strncmp(&buf[17], "V void testing::v1::detail::SHOULD_REGISTER_GTEST", 49)) {
        result.push_back(&buf[17 + 49 + 1]);
      }
    }
  }
  pclose(fp);

  return result;
}

}  // detail
}  // v1
}  // testing
