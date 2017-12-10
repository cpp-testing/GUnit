## GUnit.GTest-Lite

* Synopsis
  ```cpp
  template <class T, T...>
  constexpr auto operator""_test;

  template <class T, T...>
  constexpr auto operator""_test_disabled;
  ```

## GUnit.GTest-Lite - Tutorial by example
```cpp
int main() {
  "should always be true"_test = [] {
    EXPECT_TRUE(true);
  };

  "should not be run"_test_disabled = [] {
    EXPECT_TRUE(false);
  };
}
```
