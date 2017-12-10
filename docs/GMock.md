## GUnit.GMock
 * **GoogleMock without writing and maintaining mocks by hand**
 * Supported features
   * `EXPECT_CALL` (requires additional parens for function call)
   ```cpp
   EXPECT_CALL(mock, function(42)).WillOnce(Return(true)); // GoogleMock
   EXPECT_CALL(mock, (function)(42)).WillOnce(Return(true)); // GUnit.GMock
   EXPECT_INVOKE(mock, function, 42) // GUnit.GMock
   ```
   * `ON_CALL` (requires additional parens for function call)
   ```cpp
   ON_CALL(mock, function()).WillByDefault(Return(true)); // GoogleMock
   ON_CALL(mock, (function)()).WillByDefault(Return(true)); // GUnit.GMock
   ```
   * `Return/ReturnRef`
   * `WaggyMock/StrictMock/NiceMock`
   * Compile error when parameters and expectations don't match
   * It works together with traditional GoogleMock mocks (See [Example](https://github.com/cpp-testing/GUnit/blob/master/test/GMock.cpp))

* Synopsis
  ```cpp
  namespace testing {
    template <class T>
    class GMock {
      static_assert(std::is_abstract<T>::value, "T has to be an abstract type");
      static_assert(std::has_virtual_destructor<T>::value, "T has to have a virtual destructor");

      GMock() = default;
      GMock(GMock &&) = default;
      GMock(const GMock &) = delete;

    public:
      using type = T;

      T&() object();
      const T&() object() const;

      explicit operator T&();
      explicit operator const T&() const;
    };

    template <class T>
    using NaggyGMock = GMock<T>;

    template <class T>
    using StrictGMock = StrictMock<GMock<T>>;

    template <class T>
    using NiceGMock = NiceMock<GMock<T>>;

    /**
     * [Proposal - generic factories]
     *   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0338r0.pdf
     *
     * @tparam T type to be created ex. std::unique_ptr<example>
     * @param args arguments (converts GMock for given types T*, T&, shared_ptr, unique_ptr)
     * @return T instance
     */
    template <class T, class... TArgs>
    auto make(TArgs&&... args);
  } // testing

  template<class TMock>
  auto object(TMock&); // converts mock to the underlying type
  ```
## GUnit.GMock - Tutorial by example

```cpp
class iconfig {
 public:
  virtual bool is_dumpable() const = 0;
  virtual ~iconfig() = default;
};
```

```cpp
class iprinter {
 public:
  virtual ~iprinter() = default;
  virtual void print(const std::string& text) = 0;
};
```

```cpp
class example {
 public:
  example(const iconfig& config, const std::shraed_ptr<iprinter>& printer)
    : config(config), printer(printer)
  { }

  void update() {
    if (config.is_dumpable()) {
      printer->print("text");
    }
  }

 private:
  const iconfig& config;
  std::shared_ptr<iprinter> printer;
};
```

### Test (V1)
```cpp
TEST(Test, ShouldPrintTextWhenUpdate) {
  using namespace testing;
  GMock<iconfig> mockconfig; // defines and creates a mock
  auto mockprinter = std::make_shared<GMock<iprinter>>(); // defines and creates a mock

  example sut{static_cast<const iconfig&>(mockconfig)
            , object(mockprinter)};

  EXPECT_CALL(mockconfig, (is_dumpable)()).WillOnce(Return(true)); // additional parens
  EXPECT_CALL(*mockprinter, (print)("text")); // additional parens

  sut.update();
}
```

* (+) **NO HAND WRITTEN MOCKS**
* (-) Additional casting of mocks is required
* (~) Additional parens with a method call

> Can we do better?

### Test (V2) / using make

```cpp
TEST(Test, ShouldPrintTextWhenUpdate) {
  using namespace testing;
  StrictGMock<iconfig> mockconfig; // strict mock
  auto mockprinter = std::make_shared<StrictGMock<iprinter>>(); // strict mock

  auto sut = make<example>(mockconfig, mockprinter); // automatically converts mocks to interfaces

  EXPECT_CALL(mockconfig, (is_dumpable)()).WillOnce(Return(true));
  EXPECT_CALL(*mockprinter, (print)("text"));

  sut.update();
}
```

* (+) No castings required

> Is the make call generic?

### Test (V2.1) / using make and unique_ptr

```cpp
TEST(Test, ShouldPrintTextWhenUpdate) {
  using namespace testing;
  StrictGMock<iconfig> mockconfig;
  auto mockprinter = std::make_shared<StrictGMock<iprinter>>();

  // create a unique_ptr
  auto sut = make<std::unique_ptr<example>>(mockconfig, mockprinter);

  EXPECT_CALL(mockconfig, (is_dumpable)()).WillOnce(Return(true));
  EXPECT_CALL(*mockprinter, (print)("text"));

  sut->update();
}
```

### GMock conversion to the underlying type

```cpp
foo_ref(IFoo&);
foo_ptr(IFoo*);

int main() {
  GMock<IFoo> mock;
  foo_ref(object(mock)); // converts mock to IFoo&
  foo_ptr(object(mock)); // converts mock to IFoo*
};
```

```cpp
foo_up(std::unique_ptr<IFoo>);
foo_ref(IFoo&);
foo_ptr(IFoo*);

int main() {
  std::unique_ptr<StrictGMock<IFoo>> mock
    = std::make_unique<StrictGMock<IFoo>>();

  foo_up(object(mock));  // converts mock to std::unique_ptr<IFoo>
  foo_ref(object(mock)); // converts mock to IFoo&
  foo_ptr(object(mock)); // converts mock to IFoo*
}
```

```cpp
foo_up(std::shared_ptr<IFoo>);
foo_ref(IFoo&);
foo_ptr(IFoo*);

int main() {
  std::shared_ptr<StrictGMock<IFoo>> mock
    = std::make_shared<StrictGMock<IFoo>>();

  foo_sp(object(mock));  // converts mock to std::shared_ptr<IFoo>
  foo_ref(object(mock)); // converts mock to IFoo&
  foo_ptr(object(mock)); // converts mock to IFoo*
}
```

### How to mock overloaded methods?

```cpp
class interface {
 public:
  virtual void f(int) = 0;
  virtual void f(int) const = 0;
  virtual ~interface() = default;
};

GMock<interface> mock;

EXPECT_CALL(mock, (f, void(int) const)(1));
EXPECT_CALL(mock, (f, void(int))(2));

static_cast<const interface&>(mock).f(1);
mock.object().f(2);
```

### Universal EXPECT_* syntax (works with Google Mock's and GUnit.GMock's)

```cpp
struct IFoo {
  virtual ~IFoo() noexcept = default;
  virtual bool foo(int) = 0;
};
```
```cpp
GMock<IFoo> mock;
EXPECT_INVOKE(mock, foo, 42).WillOnce(Return(42));
// same as EXPECT_CALL(mock, (foo)(42)).WillOnce(Return(42));
```

### Mocking templates?

* Simple, just put an interface on it!

#### Example
```cpp
struct Generic {
  template<class... Ts>
  void foo(Ts...) const;
};

template<class T>
class GenericExample {
public:
  explicit GenericExample(const T&);
  void bar() {
    t.foo(42, 77.0); // call via templated object
  }

private:
  const T& t;
};

/**
 * Needed for testing but it's still better than MOCK_CONST_METHOD2
 */
struct IGeneric {
  virtual ~IGeneric() = 0;
  virtual void foo(int, double) const = 0;
};

StrictGMock<IGeneric> generic{};
GenericExample<IGeneric> sut{object(generic)};

EXPECT_CALL(generic, (foo)(42, 77.0));

sut.bar();
```

### [Advanced] Constructors with non-interface parameters and make (Assisted Injection)

```cpp
  example(iconfig& config, int value, const std::shared_ptr<iprinter>& printer, int data);
                            ^                                                   ^
                            \_____________________       _______________________/
                                                  \     /
  std::tie(sut, mocks) = make<example, StrictMock>(42, 77); // order of the same types is important
                                                            // but it's not imortant for unique types
```

### [Advanced] Generic Factories

```cpp
template <class T, class... TArgs>
struct IFactory {
  virtual T create(TArgs...) = 0;
  virtual ~IFactory() = default;
};
```

```cpp
using IConfigFactory = IFactory<IConfig, std::string>;
```

```cpp
GMock<IConfig> mockconfig;
EXPECT_CALL(mock<IConfigFactory>(), (create)("string")).WillOnce(Return(mockconfig));
```

* **(+) No specfic factory mocks for given number of parmaeters**
* (+) Factory aliases can be used to determine the mock

