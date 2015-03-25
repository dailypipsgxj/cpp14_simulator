#include <iostream>

#include "sma.h"

SMA::~SMA() {
	std::cout << "Destroying indicator: " << get_name() << std::endl;
}

void SMA::send_to_lua(std::shared_ptr<LuaScript> ls, std::vector<std::shared_ptr<Candle>> candles) {
	float avg = 0.0;

	for(auto candle: candles) {
		avg += candle->m_open_bid;
	}

	avg /= candles.size();

	ls->set_number(get_name(), avg);
}
