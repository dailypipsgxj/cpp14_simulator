#ifndef LONG_TRADE_H
#define LONG_TRADE_H

#include "../tick.h"
#include "../trade.h"

class LongTrade: public Trade {
public:
	LongTrade(unsigned int, std::shared_ptr<Tick>);
	~LongTrade();
	void close(std::shared_ptr<Tick>);
	float profit();
};

#endif
