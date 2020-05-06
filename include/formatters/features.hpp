/*
 * features.hpp
 *
 *  Created on: Feb 18, 2020
 *      Author: rbauer
 */

#ifndef TESTS_GUNITWRAPPER_FEATURES_HPP_
#define TESTS_GUNITWRAPPER_FEATURES_HPP_

#include <memory>
#include <unordered_map>

#include "gherkinCpp/feature.hpp"
#include "cucumberjson.hpp"

class Features {
public:
	virtual ~Features() {
		publishReport();
	}

	static Features* getInstance() {
		static Features instance;
		return &instance;
	}

	void addFeature(std::shared_ptr<GherkinCpp::Feature> feature) {
		if(feature == nullptr) return;
		std::string tags;
		for (auto const& tag : feature->tags) {
			tags += tag.second->name;
		}
		const std::string key = feature->name + tags;

		if(features.find(key) != features.end()) {
			std::cout << "Feature " << feature->name << " already exist." << std::endl;
			return;
		}
		features[key] = std::move(feature);
	}

	std::shared_ptr<GherkinCpp::Feature> getFeature(std::string featureName) {
		std::unordered_map<std::string, std::shared_ptr<GherkinCpp::Feature>>::iterator it = features.find(featureName);
		std::shared_ptr<GherkinCpp::Feature> retval{};

		if(it != features.end()) {
			retval = it->second;
		}
		return retval;
	}

	void addReport(std::string name) {
		if(resultReport != nullptr) return;
		resultReport = std::make_unique<Formatters::CucumberJson>(name);
	}

	void publishReport() {
	 if(resultReport != nullptr){
		  resultReport->generateReportFromFeatures(features);
		  resultReport->publishReport();
	  }
	}
private:
	Features() = default;

	std::unordered_map<std::string, std::shared_ptr<GherkinCpp::Feature>> features;
	std::unique_ptr<Formatters::CucumberJson> resultReport;
};



#endif /* TESTS_GUNITWRAPPER_FEATURES_HPP_ */
