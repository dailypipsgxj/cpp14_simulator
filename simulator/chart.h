#ifndef CHART_H
#define CHART_H

#include <string>
#include <vector>

#include "candle.h"
#include "indicator.h"
#include "lua_script.h"
#include "tick.h"
#include "utils.h"

// ===== CHART PERIOD ==============================================================================

enum ChartPeriod : int {
	M1  = 60,
	M5  = 60 * 5,
	M15 = 60 * 15,
	M30 = 60 * 30,
	H1  = 60 * 60,
	H4  = 60 * 60 * 4,
	D1  = 60 * 60 * 24,
};

ChartPeriod string_to_period(std::string);

// ===== SUBCLASSES ================================================================================

class CandlestickChart;

// ===== CHART =====================================================================================

class Chart {
protected:
	unsigned long m_num_candles;
	std::string m_name;
	std::shared_ptr<Tick> m_last_tick;
	std::vector<std::shared_ptr<Indicator>> m_indicators;
	std::vector<std::shared_ptr<Candle>> m_candles;
	ChartPeriod m_period;
	int m_ticks_processed;
	int m_candles_processed;
	std::shared_ptr<LuaScript> m_lua_interpreter;

public:
	Chart(std::string, std::string, unsigned long);
	virtual ~Chart();
	std::string get_name();
	virtual void process_tick(std::shared_ptr<Tick>) = 0;
	virtual void send_to_lua() = 0;
	void set_indicators(std::vector<std::shared_ptr<Indicator>>);
	static std::shared_ptr<Chart> parse_string(std::string);
	static std::vector<std::shared_ptr<Chart>> load_from_string(std::string);
	void add_new_candle(std::shared_ptr<Candle>);
	int seconds_per_period();
	bool has_full_data();
	void set_lua_interpreter(std::shared_ptr<LuaScript>);
};

#endif
