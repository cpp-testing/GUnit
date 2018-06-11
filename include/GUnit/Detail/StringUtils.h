//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include <sstream>
#include <string>
#include <type_traits>
#include <algorithm>
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

  constexpr auto operator[](int N) {
    using char_t = char[];
    return char_t{Chrs..., 0}[N];
  }

  constexpr auto size() { return sizeof...(Chrs); }

  template <char... Chrs_>
  constexpr auto operator+(string<Chrs_...>) {
    return string<Chrs..., Chrs_...>{};
  }
};

template <class TStr, std::size_t N, char... Chrs>
struct make_string : make_string<TStr, N - 1, TStr().chrs[N - 1], Chrs...> {};

template <class TStr, char... Chrs>
struct make_string<TStr, 0, Chrs...> {
  using type = string<Chrs...>;
};

inline void trim(std::string &txt) {
  txt.erase(0, txt.find_first_not_of(" \n\r\t"));
  txt.erase(txt.find_last_not_of(" \n\r\t") + 1);
}

inline std::vector<std::string> split(const std::string &str, char delimiter) {
  std::vector<std::string> result{};
  std::stringstream ss{str};
  std::string tok{};
  while (getline(ss, tok, delimiter)) {
    result.emplace_back(tok);
  }
  return result;
}

template <class T, std::enable_if_t<!std::is_integral<T>::value, int> = 0>
inline auto lexical_cast(const std::string &str) {
  std::remove_cv_t<std::remove_reference_t<T>> var{};
  std::istringstream iss{};
  iss.str(str);
  iss >> var;
  return var;
}

template <class T, std::enable_if_t<std::is_integral<T>::value, int> = 0>
inline auto lexical_cast(const std::string &str) {
  std::remove_cv_t<std::remove_reference_t<T>> var{};
  std::istringstream iss{};
  iss.str(str);
  if (str.size() > 2 and str[0] == '0' and str[1] == 'x') {
    iss >> std::hex >> var;
  } else {
    iss >> var;
  }
  return var;
}

template <>
inline auto lexical_cast<std::string &>(const std::string &str) {
  return str;
}

template <>
inline auto lexical_cast<const std::string &>(const std::string &str) {
  return str;
}

template <>
inline auto lexical_cast<bool>(const std::string &str) {
  std::string tmp{str};
  std::transform(tmp.begin(), tmp.end(), tmp.begin(), [](auto c) { return std::tolower(c); });
  return tmp == "1" or tmp == "true";
}

}  // detail

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wgnu-string-literal-operator-template"
#endif

template <class T, T... Chrs>
constexpr auto operator""_gtest_string() {
  return detail::string<Chrs...>{};
}

}  // v1
}  // testing

using ::testing::operator""_gtest_string;
