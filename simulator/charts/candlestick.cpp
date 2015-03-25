#include <iostream>

#include "candlestick.h"

void CandlestickChart::process_tick(std::shared_ptr<Tick> t) {
	int id = static_cast<int>(t->get_time()) / seconds_per_period();

	if(!m_candles.size()) {
		add_new_candle(std::make_shared<Candle>(id, t));
	} else {
		auto candle = m_candles[0];

		if(id > candle->get_id()) {
			candle->close(m_last_tick);
			add_new_candle(std::make_shared<Candle>(id, t));
		} else {
			candle->update(t);
		}
	}

	m_last_tick = t;
}

CandlestickChart::~CandlestickChart() {
	std::cout << "Killing candlestick chart: " << m_name << std::endl;
};

void CandlestickChart::send_to_lua() {
	// This does the Lua equivalent of:
	// element1 = {"open_bid":1.23,"open_ask":1.23,...}
	// element2 = {"open_bid":2.34,"open_ask":2.34,...}
	// table = {0 => element1, 1 => element2, ...}
	// table.send_to_lua!

	m_lua_interpreter->create_table(static_cast<int>(m_candles.size()));

	int index = 0; // current, incomplete candle is index 0. 1-60 are complete candles.

	for(auto candle: m_candles) {
		m_lua_interpreter->create_table_candle(index,
						candle->m_open_bid,
						candle->m_open_ask,
						candle->m_close_bid,
						candle->m_close_ask,
						candle->m_volume);

		index++;
	}

	m_lua_interpreter->finalize_table(get_name());
}
