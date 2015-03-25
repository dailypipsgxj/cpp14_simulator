#ifndef SMA_H
#define SMA_H

#include "../candle.h"
#include "../indicator.h"
#include "../lua_script.h"

class SMA: public Indicator {
public:
	SMA(std::string a, int b) : Indicator(a, b) {}
	~SMA();
	void send_to_lua(std::shared_ptr<LuaScript>, std::vector<std::shared_ptr<Candle>>);
};

#endif
