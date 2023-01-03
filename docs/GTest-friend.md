There already are many good resources which explains why you shouldn't test private parts of your implementation and how to avoid it.

If you want to make your classes friend with their test, there are some ways to achieve it:

* <font size="1">Add `#define private public` & `#define protected public` before including production headers to your test sources. But I never said this.</font>

* Note that you can declare your test with an existing class instead of a string :
`GTEST(SomeClass) { ... }`

* You can declare your class `friend` with the test class `GTEST(SomeClass)` :
    ```#include <GUnit/Detail/StringUtils.h>
    // Forward declare some GUnit classes
    template <typename ...> struct GTEST;

    class SomeClass {
        friend struct GTEST<SomeTest, testing::detail::string<> >;
    };

    GTEST(SomeTest) { ... }
    ```
* You can also declare friend with the test class `GTEST("SomeTest")`, though it is more cumbersome :
    ```#include <GUnit/Detail/StringUtils.h>
    // Forward declare some GUnit classes
    template <typename ...> struct GTEST;

    class SomeClass {
        friend struct GTEST<testing::detail::string<'\"', 'S', 'o', 'm', 'e', 'T', ,'e', 's', 't', '\"', '\000'>, testing::detail::string<> >;
    };

    GTEST("SomeTest") { ... }
    ```

* You can even declare friendness with `GTEST("SomeTest")` by importing some GUnit magic
```#include <GUnit/Detail/Preprocessor.h>
    #include <GUnit/Detail/StringUtils.h>

    #define FRIEND_GTEST(name) \
    struct __GUNIT_CAT(GTEST_STRING_, __LINE__) { static constexpr const char* chrs = #name; }; \
    friend struct GTEST<decltype(::make_string<__GUNIT_CAT(GTEST_STRING_, __LINE__), sizeof(#name)>::type()), ::string<>>;

    class SomeClass {
        FRIEND_GTEST("SomeTest")
    };

    GTEST("SomeTest") { ... }
```
