#include <iostream>
#include <string>

#include <boost/algorithm/string.hpp>

#include "chart.h"
#include "charts/candlestick.h"

ChartPeriod string_to_period(std::string s) {
	if("M1" == s)
		return ChartPeriod::M1;
	else if("M5" == s)
		return ChartPeriod::M5;
	else if("M15" == s)
		return ChartPeriod::M15;
	else if("M30" == s)
		return ChartPeriod::M30;
	else if("H1" == s)
		return ChartPeriod::H1;
	else if("H4" == s)
		return ChartPeriod::H4;
	else if("D1" == s)
		return ChartPeriod::D1;
	else {
		bail("Unknown period");
	}
}

// ===== CHART =====================================================================================

Chart::Chart(std::string type, std::string period, unsigned long num_candles) {
	m_name = type + "_" + period;
	m_period = string_to_period(period);
	m_num_candles = num_candles + 1; // keep an extra candle for the current, incomplete one
	m_ticks_processed = 0;
	m_last_tick = nullptr;
	m_lua_interpreter = nullptr;

	std::cout << "Created chart: " << m_name << std::endl;
}

Chart::~Chart() {
	std::cout << "Killing chart: " << m_name << std::endl;
};

void Chart::add_new_candle(std::shared_ptr<Candle> c) {
	m_candles_processed++;

	m_candles.insert(m_candles.begin(), c);

	if(m_candles.size() > m_num_candles) {
		m_candles.pop_back();
	}

	if(m_lua_interpreter && has_full_data()) {
		send_to_lua();

		for(auto indi: m_indicators) {
			indi->send_to_lua(m_lua_interpreter, m_candles);
		}
	}
}

std::string Chart::get_name() {
	return m_name;
}

bool Chart::has_full_data() {
	return m_candles.size() == m_num_candles;
}

int Chart::seconds_per_period() {
	return static_cast<int>(m_period);
}

void Chart::set_indicators(std::vector<std::shared_ptr<Indicator>> is) {
	m_indicators = is;
}

std::shared_ptr<Chart> Chart::parse_string(std::string data) {
	std::vector<std::string> parts;
	boost::split(parts, data, boost::is_any_of(","));
	trim_strings(parts);

	if(3 != parts.size()) {
		std::cout << "Data: " << data << std::endl;
		bail("Expected type,period,max_candles");
	}

	std::string name = parts[0];
	std::string period = parts[1];
	int max_candles = stoi(parts[2]);

	if("candlestick" == name) {
		return std::make_shared<CandlestickChart>(name, period, max_candles);
	} else {
		std::cout << "Chart type: " << name << std::endl;
		bail("Unknown chart type");
	}
}

std::vector<std::shared_ptr<Chart>> Chart::load_from_string(std::string data) {
	std::vector<std::shared_ptr<Chart>> charts;

	std::vector<std::string> chart_lines;
	boost::split(chart_lines, data, boost::is_any_of("\n"));
	trim_strings(chart_lines);

	if(!chart_lines.size()) {
		return charts;
	}

	for(auto it: chart_lines) {
		// skip empty lines at beginning and end of multi-line Lua std::string
		if(!it.size())
			continue;

		std::vector<std::string> line_parts;
		boost::split(line_parts, it, boost::is_any_of("|"));
		trim_strings(line_parts);

		if(2 != line_parts.size()) {
			std::cout << "Line: " << it << std::endl;
			bail("Expected chart_config|indicator_config");
		}

		auto chart = Chart::parse_string(line_parts[0]);

		// make sure similar chart is not already attached
		for(auto cit: charts) {
			if(chart->get_name() == cit->get_name()) {
				std::cout << "Name: " << chart->get_name() << std::endl;
				bail("Duplicate chart name detected");
			}
		}

		auto indis = Indicator::load_from_string(chart->get_name(), line_parts[1]);
		chart->set_indicators(indis);

		charts.push_back(chart);
	}

	return charts;
}

void Chart::set_lua_interpreter(std::shared_ptr<LuaScript> ls) {
	m_lua_interpreter = ls;
}
