#include "exploration.h"

Exploration::Exploration(std::shared_ptr<LuaScript> script,
	std::vector<std::shared_ptr<Chart>> charts) {
	m_charts = charts;
	m_script = script;

	for(auto chart: m_charts) {
		chart->set_lua_interpreter(script);
	}
}

Exploration::~Exploration() {
}

void Exploration::run(std::unique_ptr<TickSource> &ts) {
	std::cout << "Running exploration" << std::endl;

	m_script->before_run();

	for(auto tick = ts->next(); tick; tick = ts->next()) {
		for(auto chart: m_charts) {
			chart->process_tick(tick);
		}

		m_script->set_number("current_bid", tick->get_bid());
		m_script->set_number("current_ask", tick->get_ask());
		m_script->set_number("current_spread", tick->get_ask() - tick->get_bid());

		m_script->buy_signal();
		m_script->sell_signal();
	}

	m_script->after_run();
}
