/*
 * scenario.hpp
 *
 *  Created on: Feb 17, 2020
 *      Author: rbauer
 */

#ifndef TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_SCENARIO_HPP_
#define TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_SCENARIO_HPP_

#include "element.hpp"

namespace GherkinCpp {

class Scenario : public Element {
public:
	Scenario() = delete;
	Scenario(std::string name_, int line_) : Element("scenario", "Scenario", name_, line_) {};
	virtual ~Scenario() = default;

};

} // GherkinCpp

#endif /* TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_SCENARIO_HPP_ */
