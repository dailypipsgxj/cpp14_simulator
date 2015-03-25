#ifndef INDICATOR_H
#define INDICATOR_H

#include <iostream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "candle.h"
#include "lua_script.h"
#include "utils.h"

// ===== SUBCLASSES ================================================================================

class SMA;

// ===== INDICATOR =================================================================================

class Indicator
{
protected:
	std::string m_name;
	int m_num_candles;

public:
	Indicator(std::string, int);
	virtual ~Indicator();
	virtual void send_to_lua(std::shared_ptr<LuaScript>, std::vector<std::shared_ptr<Candle>>) = 0;
	const std::string get_name();
	static std::shared_ptr<Indicator> parse_string(std::string, std::string);
	static std::vector<std::shared_ptr<Indicator>> load_from_string(std::string, std::string);
};

#endif
