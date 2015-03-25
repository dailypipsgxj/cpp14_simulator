#include <iostream>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "variable.h"
#include "utils.h"

std::mt19937 new_mt() {
	std::random_device rd;
	std::mt19937 mt(rd());

	return mt;
}

// ===== VARIABLE ==================================================================================

Variable::~Variable() {
}

std::string Variable::get_name() {
	return m_name;
}

std::shared_ptr<Variable> Variable::parse_string(std::string data) {
	std::vector<std::string> parts;
	boost::split(parts, data, boost::is_any_of(","));
	trim_strings(parts);

	std::string type(parts[0]);
	std::string name(parts[1]);

	if("bool" == type) {
		if(2 != parts.size()) {
			std::cout << "Line: " << data << std::endl;
			bail("Expected bool,name");
		}

		return std::make_shared<BoolVariable>(name);
	} else if("float" == type) {
		if(4 != parts.size()) {
			std::cout << "Line: " << data << std::endl;
			bail("Expected float,name,lower,upper");
		}

		auto lower = std::stof(parts[2]);
		auto upper = std::stof(parts[3]);

		return std::make_shared<FloatVariable>(name, lower, upper);
	} else if("int" == type) {
		if(4 != parts.size()) {
			std::cout << "Line: " << data << std::endl;
			bail("Expected int,name,lower,upper");
		}

		auto lower = std::stoi(parts[2]);
		auto upper = std::stoi(parts[3]);

		return std::make_shared<IntVariable>(name, lower, upper);
	} else {
		std::cout << "Line: " << data << std::endl;
		bail("Unknown line format");
	}
}

std::vector<std::shared_ptr<Variable>> Variable::load_from_string(std::string data) {
	std::vector<std::shared_ptr<Variable>> vars;

	std::vector<std::string> variable_lines;
	boost::split(variable_lines, data, boost::is_any_of("\n"));
	trim_strings(variable_lines);

	if(!variable_lines.size()) {
		return vars;
	}

	for(auto line: variable_lines) {
		// skip empty lines at beginning and end of multi-line Lua std::string
		if(!line.size())
			continue;

		auto var = Variable::parse_string(line);

		// make sure similar variable is not already attached
		for(auto v: vars) {
			if(v->get_name() == var->get_name()) {
				std::cout << "Name: " << var->get_name() << std::endl;
				bail("Duplicate var name detected");
			}
		}

		vars.push_back(var);
	}

	return vars;
}

// ===== FLOAT =====================================================================================

FloatVariable::FloatVariable(std::string name, float lower, float upper) {
	if(lower >= upper) {
		std::cout << "Lower: " << lower << ", Upper: " << upper << std::endl;
		bail("Lower must be < Upper");
	}

	m_lower = lower;
	m_upper = upper;
	m_name = name;
}

FloatVariable::~FloatVariable() {
}

void FloatVariable::randomize() {
	std::uniform_real_distribution<float> dist(m_lower, m_upper);

	auto mt = new_mt();
	m_value = dist(mt);
}

std::string FloatVariable::show() {
	return std::to_string(m_value);
}

void FloatVariable::send_to_lua(std::shared_ptr<LuaScript> ls) {
	ls->set_number(m_name, m_value);
}

// ===== INT =======================================================================================

IntVariable::IntVariable(std::string name, int lower, int upper) {
	if(lower >= upper) {
		std::cout << "Lower: " << lower << ", Upper: " << upper << std::endl;
		bail("Lower must be < Upper");
	}

	m_lower = lower;
	m_upper = upper;
	m_name = name;
}

IntVariable::~IntVariable() {
}

void IntVariable::randomize() {
	std::uniform_int_distribution<int> dist(m_lower, m_upper);

	auto mt = new_mt();
	m_value = dist(mt);
}

std::string IntVariable::show() {
	return std::to_string(m_value);
}

void IntVariable::send_to_lua(std::shared_ptr<LuaScript> ls) {
	ls->set_number(m_name, m_value);
}

// ===== BOOL ======================================================================================

BoolVariable::BoolVariable(std::string name) {
	m_name = name;
}

BoolVariable::~BoolVariable() {
}

void BoolVariable::randomize() {
	std::uniform_int_distribution<int> dist(0, 1);

	auto mt = new_mt();
	m_value = (0 == dist(mt));
}

std::string BoolVariable::show() {
	return std::string(m_value ? "true" : "false");
}

void BoolVariable::send_to_lua(std::shared_ptr<LuaScript> ls) {
	ls->set_bool(m_name, m_value);
}
