#pragma once

#include <limits>
#include <list>

class Calculator {
private:
    std::list<double> values;
public:
    void push(double);
    double add();
    double divide();
};

void Calculator::push(double n) {
    values.push_back(n);
}

double Calculator::add() {
    double result = 0;
    for(std::list<double>::const_iterator i = values.begin(); i != values.end(); ++i) {
        result += *i;
    }
    return result;
}

double Calculator::divide() {
    double result = std::numeric_limits<double>::quiet_NaN();
    for(std::list<double>::const_iterator i = values.begin(); i != values.end(); ++i) {
        if (i == values.begin()) {
            result = *i;
        } else {
            result /= *i;
        }
    }
    return result;
}
