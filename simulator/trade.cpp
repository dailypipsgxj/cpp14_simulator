#include "trade.h"

Trade::Trade(unsigned int id, std::shared_ptr<Tick> tick) {
	record_tick(tick);

	m_id = id;
	m_open = true;

	m_open_bid = tick->get_bid();
	m_open_ask = tick->get_ask();

	m_opened_at = tick->get_time();
}

Trade::~Trade() { }

void Trade::record_tick(std::shared_ptr<Tick> tick) {
	m_ticks.push_back(tick);
}

std::shared_ptr<Tick> Trade::get_last_tick() {
	return m_ticks.back();
}

unsigned long Trade::time_open() {
	unsigned long opened_at = static_cast<unsigned long>(m_opened_at);

	if(m_open) {
		return static_cast<unsigned long>(get_last_tick()->get_time()) - opened_at;
	} else {
		return static_cast<unsigned long>(m_closed_at) - opened_at;
	}
}
