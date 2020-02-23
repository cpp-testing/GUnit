/*
 * tag.hpp
 *
 *  Created on: Feb 17, 2020
 *      Author: rbauer
 */

#ifndef TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_TAG_HPP_
#define TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_TAG_HPP_

#include <string>
#include <memory>

namespace GherkinCpp {

class Tag {
public:
	Tag() = delete;
	Tag(std::string name_, int line_) : name(name_), line(line_) {};
	virtual ~Tag() = default;

public:
	const std::string name;
	const int line;
};

} // GherkinCpp

#endif /* TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_TAG_HPP_ */
