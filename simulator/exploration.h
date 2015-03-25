#ifndef EXPLORATION_H
#define EXPLORATION_H

#include "chart.h"
#include "lua_script.h"
#include "tick_source.h"
#include "trade.h"

class Exploration {
	std::shared_ptr<LuaScript> m_script;
	std::vector<std::shared_ptr<Chart>> m_charts;
public:
	Exploration(std::shared_ptr<LuaScript>, std::vector<std::shared_ptr<Chart>>);
	~Exploration();
	void run(std::unique_ptr<TickSource> &);
};

#endif
