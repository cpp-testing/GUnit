/*
 * step.hpp
 *
 *  Created on: Feb 17, 2020
 *      Author: rbauer
 */

#ifndef TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_STEP_HPP_
#define TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_STEP_HPP_

#include <unordered_map>

#include "genericinfo.hpp"

namespace GherkinCpp {

class Step : public GenericInfo{
public:
	Step() = delete;
	Step(std::string keyword_, std::string name_, int line_) : GenericInfo(keyword_ , name_, line_) {};
	virtual ~Step() = default;

	void setResult(bool result_) {
		result = result_;
	}

	bool getResult() { return result;}

private:
	bool result=true;
};

} // GherkinCpp

#endif /* TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_STEP_HPP_ */
