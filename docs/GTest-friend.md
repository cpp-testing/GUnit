There already are many good resources which explains why you shouldn't test private parts of your implementation and how to avoid it.

If you want to make make your classes friend with their test, here are some ways to achieve it:

* <font size="1">Add `#define private public` & `#define protected public` before including production headers to your test sources. But I never said this.</font>

* You can declare your test with an existing class instead of a string :
`GTEST(SomeClass) { ... }`
With this syntax, the test will inherit from `SomeClass`, so you can access to protected members.

    This is the least intrusive way, as there is nothing added to the tested class - apart from maybe replace private by protected.

* You can declare your class `friend` with the test class `GTEST(SomeClass)` :
    ```// Forward declare some GUnit classes
    template <typename ...> struct GTEST;
    namespace testing::detail { template <char ...> struct string; }
    
    class SomeClass {
        friend struct GTEST<SomeClass, testing::detail::string<> >;
    };

    GTEST(Game) { ... }
    ```
* You can also declare friend with the test class `GTEST("SomeTest")`, it is more cumbersome :
    ```// Forward declare some GUnit classes
    template <typename ...> struct GTEST;
    namespace testing::detail { template <char ...> struct string; }

    class SomeClass {
        friend struct GTEST<testing::detail::string<'\"', 'S', 'o', 'm', 'e', 'T', ,'e', 's', 't', '\"', '\000'>, testing::detail::string<> >;
    };

    GTEST("SomeTest") { ... }
    ```

* You can even declare friendness with `GTEST("SomeTest")` by importing some GUnit magic - but we're polluting our production code:
```
    #ifndef __GUNIT_CAT
    #define __GUNIT_PRIMITIVE_CAT(arg, ...) arg##__VA_ARGS__
    #define __GUNIT_CAT(arg, ...) __GUNIT_PRIMITIVE_CAT(arg, __VA_ARGS__)
    #endif
    namespace {
        template <char ...> struct string {};
        template <class TStr, std::size_t N, char... Chrs> struct make_string : make_string<TStr, N - 1, TStr().chrs[N - 1], Chrs...> {};
        template <class TStr, char... Chrs> struct make_string<TStr, 0, Chrs...> { using type = string<Chrs...>; };
    }

    #define FRIEND_GTEST(name) \
    struct __GUNIT_CAT(GTEST_STRING_, __LINE__) { static constexpr const char* chrs = #name; }; \
    friend struct GTEST<decltype(::make_string<__GUNIT_CAT(GTEST_STRING_, __LINE__), sizeof(#name)>::type()), ::string<>>;

    class SomeClass {
        FRIEND_GTEST("SomeTest")
    };

    GTEST("SomeTest") { ... }
```
