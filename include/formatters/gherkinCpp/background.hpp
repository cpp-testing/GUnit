/*
 * background.hpp
 *
 *  Created on: Feb 17, 2020
 *      Author: rbauer
 */

#ifndef TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_BACKGROUND_HPP_
#define TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_BACKGROUND_HPP_

#include "element.hpp"

namespace GherkinCpp {

class Background : public Element {
public:
	Background() = delete;
	Background(int line_) : Element("Background", "Background", "background", line_) {};
	virtual ~Background() = default;

};

} // GherkinCpp

#endif /* TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_BACKGROUND_HPP_ */
