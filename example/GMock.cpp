//
// Copyright (c) 2016-2017 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <gtest/gtest.h>
#include <stdexcept>

#include "GUnit/GMake.h"
#include "GUnit/GMock.h"

class interface {
 public:
  virtual bool get() const = 0;
  virtual ~interface() = default;
};

class extended_interface : public interface {
 public:
  virtual void foo(bool) = 0;
  virtual void bar(bool) = 0;
};

class example {
 public:
  example(const interface& i, extended_interface& ei) : i(i), ei(ei) {}

  void update() {
    const auto value = i.get();
    if (value) {
      ei.foo(value);
    } else {
      ei.bar(value);
    }
  }

 private:
  const interface& i;
  extended_interface& ei;
};

////////////////////////////////////////////////////////////////////////////////

TEST(GMock, ShouldMockSimpleInterface) {
  using namespace testing;
  GMock<interface> mock;
  EXPECT_CALL(mock, (get)()).WillOnce(Return(true));

  EXPECT_TRUE(static_cast<interface&>(mock).get());
}

TEST(GMock, ShouldMockExtendedInterface) {
  using namespace testing;
  GMock<interface> imock;
  GMock<extended_interface> emock;

  EXPECT_CALL(imock, (get)()).WillOnce(Return(true));
  EXPECT_CALL(emock, (foo)(true)).Times(1);

  example e{static_cast<const interface&>(imock),
            static_cast<extended_interface&>(emock)};
  e.update();
}

TEST(GMock, ShouldMockExtendedInterfaceDifferentMockTypes) {
  using namespace testing;
  NiceGMock<interface> imock;
  StrictGMock<extended_interface> emock;

  EXPECT_CALL(imock, (get)()).WillOnce(Return(false));
  EXPECT_CALL(emock, (bar)(false)).Times(1);

  example e{static_cast<const interface&>(imock),
            static_cast<extended_interface&>(emock)};
  e.update();
}

TEST(GMock, ShouldWorkWithMacroDefinedMocks) {
  using namespace testing;
  NiceGMock<interface> imock;
  struct mock_extended_interface : extended_interface {
    MOCK_CONST_METHOD0(get, bool());
    MOCK_METHOD1(foo, void(bool));
    MOCK_METHOD1(bar, void(bool));
  } emock;

  EXPECT_CALL(imock, (get)()).WillOnce(Return(false));
  EXPECT_CALL(emock, bar(false)).Times(1);

  example e{static_cast<const interface&>(imock),
            static_cast<extended_interface&>(emock)};
  e.update();
}

TEST(GMock, ShouldMakeExample) {
  using namespace testing;
  GMock<interface> imock;
  GMock<extended_interface> emock;

  EXPECT_CALL(imock, (get)()).WillOnce(Return(false));
  EXPECT_CALL(emock, (bar)(false)).Times(1);

  auto sut = make<example>(imock, emock);

  sut.update();
}

TEST(GMock, ShouldMakeUniquePtrExample) {
  using namespace testing;
  GMock<interface> imock;
  GMock<extended_interface> emock;

  EXPECT_CALL(imock, (get)()).WillOnce(Return(false));
  EXPECT_CALL(emock, (bar)(false)).Times(1);

  auto sut = make<std::unique_ptr<example>>(imock, emock);

  sut->update();
}

TEST(GMock, ShouldMakeSharedPtrExample) {
  using namespace testing;
  GMock<interface> imock;
  GMock<extended_interface> emock;

  EXPECT_CALL(imock, (get)()).WillOnce(Return(false));
  EXPECT_CALL(emock, (bar)(false)).Times(1);

  auto sut = make<std::shared_ptr<example>>(imock, emock);

  sut->update();
}

TEST(GMock, ShouldFailDueToUninterestingGetCall) {
  using namespace testing;
  StrictGMock<interface> mock;
  EXPECT_CALL(mock, (get)())
      .WillOnce(Return(true));  // Comment to get an UNINTERESTING CALL
  static_cast<interface&>(mock).get();
}

TEST(GMock, ShouldNotCompileWhenMethodParametersDontMatch) {
  using namespace testing;
  GMock<interface> mock;
  // EXPECT_CALL(mock, (get)(_, _)).WillOnce(Return(true)); // COMPILE ERROR
}

TEST(GMock, ShouldNotCompileWhenGMockNotUsedWithBrackets) {
  using namespace testing;
  GMock<interface> mock;
  // EXPECT_CALL(mock, get()).WillOnce(Return(false)); // COMPILE ERROR
}
