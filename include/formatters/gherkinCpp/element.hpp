/*
 * element.hpp
 *
 *  Created on: Feb 17, 2020
 *      Author: rbauer
 */

#ifndef TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_ELEMENT_HPP_
#define TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_ELEMENT_HPP_

#include <unordered_map>

#include "step.hpp"
#include "genericinfo.hpp"
#include "tag.hpp"

namespace GherkinCpp {

class Element : public GenericInfo {
public:
	Element() = delete;
	Element(std::string type_, std::string keyword_, std::string name_, int line_) : GenericInfo(keyword_, name_, line_), type(type_) {};
	virtual ~Element() = default;

	void addStep(std::shared_ptr<Step> step) {
		if(step == nullptr) return;

		int stepMapKey = step->line;

		if(steps.find(stepMapKey) != steps.end()) {
			std::cout << "Scenario " << name << ": Step " << step->name << " in line " << step->line << " already exist." << std::endl;
			return;
		}

		steps[stepMapKey] = std::move(step);
	}

	std::map<int, std::shared_ptr<Step>> getSteps() {
		return steps;
	}

	std::shared_ptr<Step> getSpecificStep(int lineNum) {
		std::map<int, std::shared_ptr<Step>>::iterator it = steps.find(lineNum);
		std::shared_ptr<Step> retval{};

		if(it != steps.end()) {
			retval = it->second;
		}
		return retval;
	}

public:
	const std::string type;

private:
	std::map<int, std::shared_ptr<Step>>  steps;
	std::string description{};
};

} // GherkinCpp

#endif /* TESTS_GUNIT_LIBS_GHERKIN_CPP_INCLUDE_ELEMENT_HPP_ */
