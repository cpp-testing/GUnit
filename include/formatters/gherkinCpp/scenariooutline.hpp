/*
 * scenario.hpp
 *
 *  Created on: Feb 17, 2020
 *      Author: rbauer
 */

#ifndef TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_SCENARIOOUTLINE_HPP_
#define TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_SCENARIOOUTLINE_HPP_

#include "element.hpp"

namespace GherkinCpp {

class ScenarioOutline : public Element {
public:
	ScenarioOutline() = delete;
	ScenarioOutline(std::string name_, int line_) : Element("scenario", "Scenario Outline", name_, line_) {};
	virtual ~ScenarioOutline() = default;

};

} // GherkinCpp

#endif /* TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_SCENARIOOUTLINE_HPP_ */
