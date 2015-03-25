#ifndef CANDLE_H
#define CANDLE_H

#include "tick.h"

class Candle
{
public:
	float m_open_bid;
	float m_open_ask;
	float m_close_bid;
	float m_close_ask;
	float m_high_bid;
	float m_high_ask;
	float m_low_bid;
	float m_low_ask;
	int m_volume;
	int m_id;
public:
	Candle(int, std::shared_ptr<Tick>);
	void close(std::shared_ptr<Tick>);
	int get_id();
	void update(std::shared_ptr<Tick>);
};

#endif
