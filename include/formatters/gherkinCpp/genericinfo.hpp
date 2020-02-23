/*
 * genericinfo.hpp
 *
 *  Created on: Feb 17, 2020
 *      Author: rbauer
 */

#ifndef TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_GENERICINFO_HPP_
#define TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_GENERICINFO_HPP_

#include <string>
#include <memory>
#include <unordered_map>
#include "tag.hpp"

namespace GherkinCpp {

class GenericInfo {
public:
	GenericInfo() = delete;
	GenericInfo(std::string keyword_, std::string name_, int line_) : name(name_), keyword(keyword_), line(line_) {};
	virtual ~GenericInfo() = default;

	std::unordered_map<std::string, std::shared_ptr<Tag>> getTags() {
		return tags;
	}

	void addTag(std::shared_ptr<Tag> tag) {
		if(tag == nullptr) return;
		if(tags.find(tag->name) != tags.end()) {
			std::cout << "Item " << name << ": Tag " << tag->name << " already exist." << std::endl;
			return;
		}

		tags[tag->name] = std::move(tag);
	}

public:
	const std::string name;
	const std::string keyword;
	const int line;
	std::unordered_map<std::string, std::shared_ptr<Tag>> tags;
};

} // GherkinCpp
#endif /* TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_GENERICINFO_HPP_ */
