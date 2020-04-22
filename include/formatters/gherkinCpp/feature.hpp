/*
 * feature.hpp
 *
 *  Created on: Feb 17, 2020
 *      Author: rbauer
 */

#ifndef TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_FEATURE_HPP_
#define TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_FEATURE_HPP_

#include <unordered_map>
#include <string>
#include "genericinfo.hpp"
#include "element.hpp"

namespace GherkinCpp {

class Feature : public GenericInfo {
public:
	Feature() = delete;
	Feature(std::string name_, std::string uri_, int line_) : GenericInfo("Feature", name_, line_), uri(uri_){};
	virtual ~Feature() = default;

	void addElement(std::shared_ptr<Element> element) {
		if(element == nullptr) return;

	   /*
		* To keep the key unique in case a scenario is present at the same line in other file. The key is created
		* by concatenating the line number with the scenario name.
		*/
		std::string elementKey = element->name + " " + std::to_string(element->line);

		if(elements.find(elementKey) != elements.end()) {
			std::cout << "Feature " << name << ": Element " << element->name << " at line " << element->line << " already exist." << std::endl;
			return;
		}
		elements[elementKey] = std::move(element);
	}

	std::unordered_map<std::string, std::shared_ptr<Element>>  getElements() {
		return elements;
	}

	std::shared_ptr<Element> getSpecificElement(std::string nameAndLine) {
		std::unordered_map<std::string, std::shared_ptr<Element>>::iterator it = elements.find(nameAndLine);
		std::shared_ptr<Element> retval{};

		if(it != elements.end()) {
			retval = it->second;
		}
		return retval;
	}

public:
	const std::string uri;

private:
	std::unordered_map<std::string, std::shared_ptr<Element>>  elements;
	std::string description{};
};

} // GherkinCpp

#endif /* TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_FEATURE_HPP_ */
