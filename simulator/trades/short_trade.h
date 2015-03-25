#ifndef SHORT_TRADE_H
#define SHORT_TRADE_H

#include "../tick.h"
#include "../trade.h"

class ShortTrade: public Trade {
public:
	ShortTrade(unsigned int, std::shared_ptr<Tick>);
	~ShortTrade();
	void close(std::shared_ptr<Tick>);
	float profit();
};

#endif
