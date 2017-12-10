#pragma once

#include <limits>
#include <list>

class Calculator {
 public:
  Calculator() = default;
  explicit Calculator(double){};

  void push(double);
  double add();
  double divide();

 private:
  std::list<double> values{};
};

struct IDisplay {
  virtual ~IDisplay() = default;
  virtual void show(double) = 0;
};

class CalculatorUI : public Calculator {
 public:
  explicit CalculatorUI(IDisplay& display) : display(display) {}

  void add() { display.show(Calculator::add()); }

  void divide() { display.show(Calculator::divide()); }

 private:
  IDisplay& display;
};

void Calculator::push(double n) { values.push_back(n); }

double Calculator::add() {
  auto result = 0.;
  for (const auto& v : values) {
    result += v;
  }
  return result;
}

double Calculator::divide() {
  double result = std::numeric_limits<double>::quiet_NaN();
  for (std::list<double>::const_iterator i = values.begin(); i != values.end();
       ++i) {
    if (i == values.begin()) {
      result = *i;
    } else {
      result /= *i;
    }
  }
  return result;
}
