#include "indicator.h"
#include "indicators/sma.h"
#include "utils.h"

Indicator::Indicator(std::string name, int num_candles) {
	m_name = name;
	m_num_candles = num_candles;

	std::cout << "Created indicator: " << m_name << std::endl;
}

Indicator::~Indicator() {
	std::cout << "Destroyed indicator: " << m_name << std::endl;
}

const std::string Indicator::get_name() {
	return m_name;
}

std::shared_ptr<Indicator> Indicator::parse_string(std::string chart_name, std::string data) {
	std::vector<std::string> parts;
	boost::split(parts, data, boost::is_any_of(","));
	trim_strings(parts);

	if(2 != parts.size()) {
		std::cout << "Data: " << data << std::endl;
		bail("Expected type,max_candles");
	}

	std::string indi_type = parts[0];
	std::string name = chart_name + "_" + indi_type + "_" + parts[1];
	int num_candles = stoi(parts[1]);

	if("sma" == indi_type) {
		return std::make_shared<SMA>(name, num_candles);
	} else {
		std::cout << "Indi type: " << indi_type << std::endl;
		bail("Unknown indi type");
	}
}

std::vector<std::shared_ptr<Indicator>> Indicator::load_from_string(std::string chart_name, std::string data) {
	std::vector<std::shared_ptr<Indicator>> indicators;

	std::vector<std::string> indicator_lines;
	boost::split(indicator_lines, data, boost::is_any_of("\n"));
	trim_strings(indicator_lines);

	if(!indicator_lines.size()) {
		return indicators;
	}

	for(auto it: indicator_lines) {
		// skip empty lines at beginning and end of multi-line Lua std::string
		if(!it.size())
			continue;

		std::vector<std::string> line_parts;
		boost::split(line_parts, it, boost::is_any_of(":"));
		trim_strings(line_parts);

		if(!line_parts.size())
			bail("Require at least one indicator per chart");

		if(1 == line_parts.size()) {
			std::cout << "Line: " << it << std::endl;
		}

		auto indicator = Indicator::parse_string(chart_name, line_parts[0]);

		// make sure similar chart is not already attached
		for(auto iit: indicators) {
			if(indicator->get_name() == iit->get_name()) {
				std::cout << "Name: " << indicator->get_name() << std::endl;
				bail("Duplicate indicator name detected");
			}
		}

		indicators.push_back(indicator);
	}

	return indicators;
}
