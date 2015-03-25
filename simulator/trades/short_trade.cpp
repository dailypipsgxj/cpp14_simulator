#include "short_trade.h"

ShortTrade::ShortTrade(unsigned int id, std::shared_ptr<Tick> tick): Trade(id, tick) {
	m_filled_open_price = tick->get_bid();
}

ShortTrade::~ShortTrade() { }

void ShortTrade::close(std::shared_ptr<Tick> tick) {
	m_open = false;
	record_tick(tick);

	m_filled_close_price = tick->get_ask();
	m_closed_at = tick->get_time();
	m_close_bid = tick->get_bid();
	m_close_ask = tick->get_ask();
}

float ShortTrade::profit() {
	if(m_open) {
		return get_last_tick()->get_ask() - m_filled_open_price;
	} else {
		return m_filled_close_price - m_filled_open_price;
	}
}
