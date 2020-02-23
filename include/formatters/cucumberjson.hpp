/*
 * cucumberjson.hpp
 *
 *  Created on: Feb 18, 2020
 *      Author: rbauer
 */

#ifndef TESTS_GUNITWRAPPER_FORMATTERS_CUCUMBERJSON_HPP_
#define TESTS_GUNITWRAPPER_FORMATTERS_CUCUMBERJSON_HPP_

#include <json.hpp>
#include <gherkin.hpp>

#include <fstream>
#include <unordered_map>

namespace Formatters {

// reference of Json output comes from https://relishapp.com/cucumber/cucumber/docs/formatters/json-output-formatter
class CucumberJson {
public:
	CucumberJson() = delete;
	CucumberJson(std::string name_) : name(name_) {};
	virtual ~CucumberJson() = default;

	void generateReportFromFeatures(const std::unordered_map<std::string, std::shared_ptr<GherkinCpp::Feature>> features) {
		for (const auto& feature : features) {
			nlohmann::json curFeature;
			curFeature["uri"] = feature.second->uri;
			curFeature["keyword"] = feature.second->keyword;
			curFeature["name"] = feature.second->name;
			curFeature["line"] = feature.second->line;
			curFeature["description"] = "";

			std::unordered_map<std::string, std::shared_ptr<GherkinCpp::Tag>> featureTags = feature.second->getTags();

			for (const auto& tag : featureTags) {
				curFeature["tags"].push_back({
						{"name", tag.second->name},
						{"line", tag.second->line}
				});
			}

			for (const auto& element : feature.second->getElements()) {
				nlohmann::json curElement;
				curElement["keyword"] = element.second->keyword;
				curElement["name"] = element.second->name;
				curElement["line"] = element.second->line;
			    curElement["description"] = "";
			    curElement["type"] = element.second->type;

			    std::unordered_map<std::string, std::shared_ptr<GherkinCpp::Tag>> elementTags = element.second->getTags();
				for (const auto& tag : elementTags) {
					curElement["tags"].push_back({
							{"name", tag.second->name},
							{"line", tag.second->line}
					});
				}

				for (const auto& step : element.second->getSteps()) {
					nlohmann::json curStep;
					curStep["keyword"] = step.second->keyword;
					curStep["name"] = step.second->name;
					curStep["line"] = step.second->line;
					std::string resultStr = step.second->getResult()?"passed":"failed";
					curStep["result"] = {
							{"status", resultStr},
							{"duration", 1}
					};
					curElement["steps"].push_back(curStep);
				}

				curFeature["elements"].push_back(curElement);
			}

			jsonReport.push_back(curFeature);
		}
	}

	void publishReport() {
		std::string fileName = name + ".json";
		std::ofstream output(fileName);
		output << std::setw(4) << jsonReport <<std::endl;
	}

private:

private:
	const std::string name;
	nlohmann::json jsonReport{};
};

} //testing



#endif /* TESTS_GUNITWRAPPER_FORMATTERS_CUCUMBERJSON_HPP_ */
