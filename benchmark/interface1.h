#pragma once

struct interface1 {
  virtual ~interface1() = default;
  virtual bool f1(int) const = 0;
};
