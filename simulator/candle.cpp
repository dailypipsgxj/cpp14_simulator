#include "candle.h"

Candle::Candle(int id, std::shared_ptr<Tick> t) {
	m_open_bid = t->get_bid();
	m_open_ask = t->get_ask();
	m_high_bid = t->get_bid();
	m_high_ask = t->get_ask();
	m_low_bid  = t->get_bid();
	m_low_ask  = t->get_ask();

	m_id = id;
}

void Candle::close(std::shared_ptr<Tick> t) {
	m_close_bid = t->get_bid();
	m_close_ask = t->get_ask();
}

int Candle::get_id() {
	return m_id;
}

void Candle::update(std::shared_ptr<Tick> t) {
        if(t->get_bid() > m_high_bid) {
            m_high_bid = t->get_bid();
        } else if(t->get_bid() < m_low_bid) {
            m_low_bid = t->get_bid();
        }

        if(t->get_ask() > m_high_ask) {
            m_high_ask = t->get_ask();
        } else if(t->get_ask() < m_low_ask) {
            m_low_ask = t->get_ask();
        }

        m_volume++;
}
