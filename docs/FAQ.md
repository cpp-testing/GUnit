## FAQ

* How `GMock` works?
  * It's not a C++ standard solution (depends on vtable implementation)
    * [Itanium C++ ABI (vtable)](https://mentorembedded.github.io/cxx-abi/abi.html)
    * [VTable in GCC](http://stackoverflow.com/questions/6258559/what-is-the-vtt-for-a-class)
    * [Deleting Destructors](http://eli.thegreenplace.net/2015/c-deleting-destructors-and-virtual-operator-delete)
    * [Devirtualization in C++](http://hubicka.blogspot.com/2014/01/devirtualization-in-c-part-1.html)
    * [Member Function Pointers and the Fastest Possible C++ Delegates](https://www.codeproject.com/kb/cpp/fastdelegate.aspx)
    * [Reversing C++ Virtual Functions](https://alschwalm.com/blog/static/2016/12/17/reversing-c-virtual-functions)
    * [C++ vtables](http://shaharmike.com/cpp/vtable-part1)
    * ```g++ -fdump-class-hierarchy interface.hpp```

  * Similar projects ([FakeIt](https://github.com/eranpeer/FakeIt), [HippoMocks](https://github.com/dascandy/hippomocks))

* How quick is `GMock`?
  * Compile time [benchmark (Example)](https://github.com/cpp-testing/GUnit/tree/master/benchmark)

      | Compiler | Number of Mocks | GoogleMock/GoogleTest | GoogleMock/GoogleTest + GUnit |
      | -------- | --------------- | --------------------- | ----------------------------- |
      | GCC-6    |               3 |                  2.6s |                         2.1s  |
      | Clang-3.9|               3 |                  2.3s |                         1.9s  |

* But virtual function call has performance overhead?
  * This statement is not really true anymore with modern compilers as most virtual calls might be inlined
    * [Devirtualization in C++](http://hubicka.blogspot.co.uk/2014/01/devirtualization-in-c-part-2-low-level.html)
    * [Using final - C++11](https://godbolt.org/g/ASLk4B)
    * [Link Time Optimization - LTO](http://hubicka.blogspot.co.uk/2014/04/linktime-optimization-in-gcc-1-brief.html)

* Limitations

  * GMock can't mock classes with multiple or virtual inheritance
  * GMock, by default, can fake interface with up to 128 virtual methods

* Integration tests with Dependency Injection ([[Boost].DI](https://github.com/boost-experimental/di))

```cpp
class example; // System Under Test

GTEST(example) {
  namespace di = boost::di;

  SHOULD("create example") {
    const auto injector = di::make_injector(
      di::bind<interface>.to(di::NiceGMock{mocks})
    , di::bind<interface2>.to(di::StrictGMock{mocks})
    );

    sut = testing::make<SUT>(injector);

    EXPECT_CALL(mock<interface>(), (get)(_)).WillOnce(Return(123));
    EXPECT_CALL(mock<interface2>(), (f2)(123));

    sut->update();
  }
}
```
