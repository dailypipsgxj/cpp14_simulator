#include "simulation.h"

#include <iostream>

#include "trades/long_trade.h"
#include "trades/short_trade.h"

Simulation::Simulation(std::shared_ptr<LuaScript> script,
	std::vector<std::shared_ptr<Chart>> charts,
	std::vector<std::shared_ptr<Variable>> variables) {
	m_charts = charts;
	m_script = script;
	m_variables = variables;

	m_score = 0.0;

	m_worst_drawdown = 0.0;
	m_drawdown_limit = -(script->drawdown_limit());

	m_deposit = script->initial_deposit();
	m_last_equity_high = m_deposit;
	m_last_equity_low = m_deposit;
	m_equity_high = m_deposit;
	m_equity_low = m_deposit;

	m_total_trade_count = 0;

	for(auto chart: m_charts) {
		chart->set_lua_interpreter(script);
	}

	// std::cout << "Created simulation" << std::endl;
}

Simulation::~Simulation() {
	// std::cout << "Destroyed simulation" << std::endl;
	// std::cout << "Total trades: " << m_total_trade_count << std::endl;
}

void Simulation::run(std::unique_ptr<TickSource> &ts) {
	std::cout << "Running simulation" << std::endl;

	std::shared_ptr<Tick> last_tick = nullptr;

	for(auto tick = ts->next(); tick; tick = ts->next()) {
		for(auto trade: m_open_trades) {
			trade->record_tick(tick);
		}

		for(auto chart: m_charts) {
			chart->process_tick(tick);
		}

		update_stats();
		if(has_exceeded_max_drawdown()) {
			std::cout << "Drawdown: " << m_worst_drawdown << std::endl;
			std::cout << "Limit: " << m_drawdown_limit << std::endl;
			bail("Exceeded max drawdown");
		}

		m_script->set_number("current_bid", tick->get_bid());
		m_script->set_number("current_ask", tick->get_ask());
		m_script->set_number("current_spread", tick->get_ask() - tick->get_bid());
		m_script->set_bool("has_open_trades", m_open_trades.size() > 0);

		int decision = m_script->on_tick();

		if(BUY == decision) {
			open_long_trade(tick);
		} else if(SELL == decision) {
			open_short_trade(tick);
		} else if(CLOSE == decision) {
			close_all_trades(tick);
		} else if(NOOP == decision) {
			// do nothing
		} else {
			std::cout << "Decision: " << decision << std::endl;
			bail("Received unknown decision");
		}

		last_tick = tick;
	}

	close_all_trades(last_tick);
}

float Simulation::get_score() {
	// TODO: remove this * 1000
	return get_profit() * 1000;
}

void Simulation::open_long_trade(std::shared_ptr<Tick> tick) {
	m_total_trade_count++;

	auto t = std::make_shared<LongTrade>(m_total_trade_count, tick);
	m_open_trades.push_back(t);
}

void Simulation::open_short_trade(std::shared_ptr<Tick> tick) {
	m_total_trade_count++;

	auto t = std::make_shared<ShortTrade>(m_total_trade_count, tick);
	m_open_trades.push_back(t);
}

void Simulation::close_all_trades(std::shared_ptr<Tick> tick) {
	if(!m_open_trades.size()) {
		return;
	}

	// int count = 0;
	// std::cout << m_open_trades.size() << " items to move" << std::endl;

	for(auto trade = m_open_trades.begin(); trade != m_open_trades.end(); ) {
		auto t = *trade;
		t->close(tick);
		m_closed_trades.push_back(t);
		trade = m_open_trades.erase(trade);
		// count++;
	}

	// std::cout << "Moved " << count << " items" << std::endl;
}

// "balance" is the sum of deposit + closed trades
float Simulation::get_balance() {
	float value = m_deposit;

	for(auto trade: m_closed_trades) {
		value += trade->profit();
	}

	return value;
}

// "equity" is the sum of current balance + profit/loss of open trades
float Simulation::get_equity() {
	float value = get_balance();

	for(auto trade: m_open_trades) {
		value += trade->profit();
	}

	return value;
}

// balance - deposit
float Simulation::get_profit() {
	return get_balance() - m_deposit;
}

// profit() / number of closed trades
float Simulation::expectancy_per_trade() {
	return get_profit() / m_closed_trades.size();
}

float Simulation::percentage_of_profitable_trades() {
	if(!m_closed_trades.size())
		return 0.0;

	unsigned long num_profitable = 0;

	for(auto trade: m_closed_trades) {
		if(trade->profit() > 0.0)
			num_profitable++;
	}

	// std::cout << "Num trades: " << m_closed_trades.size() << std::endl;
	// std::cout << "Profitable: " << num_profitable << std::endl;

	float per = static_cast<float>(num_profitable) / static_cast<float>(m_closed_trades.size());

	return per * 100.0f;
}

float Simulation::average_trade_duration() {
	float val = 0.0;

	for(auto trade: m_closed_trades) {
		val += trade->time_open();
	}

	return val / static_cast<float>(m_closed_trades.size());
}

void Simulation::update_stats() {
	float equity = get_equity();

	if(equity > m_equity_high) {
		m_equity_high = equity;
	} else if(equity < m_equity_low) {
		m_equity_low = equity;
	}

	if(equity > m_last_equity_high) {
		m_last_equity_high = equity;
		m_last_equity_low  = equity;
	} else if(equity <= m_last_equity_high) {
		m_last_equity_low = equity;
	}

	float drawdown = -(100.0f - ((m_last_equity_low / m_last_equity_high) * 100.0f));

	// std::cout << "Drawdown: " << drawdown << std::endl;

	if(drawdown < m_worst_drawdown) {
		m_worst_drawdown = drawdown;
	}
}

float Simulation::get_worst_drawdown() {
	return m_worst_drawdown;
}

bool Simulation::has_exceeded_max_drawdown() {
	return (m_worst_drawdown < m_drawdown_limit);
}

float Simulation::get_equity_high() {
	return m_equity_high;
}

float Simulation::get_equity_low() {
	return m_equity_low;
}

unsigned long Simulation::get_trade_count() {
	return m_closed_trades.size() + m_open_trades.size();
}

unsigned long Simulation::get_winning_trade_count() {
	unsigned long count = 0;

	for(auto trade: m_closed_trades) {
		if(trade->profit() >= 0.0f) {
			count++;
		}
	}

	return count;
}

unsigned long Simulation::get_losing_trade_count() {
	return get_trade_count() - get_winning_trade_count();
}

std::shared_ptr<Trade> Simulation::best_winning_trade() {
	if(!m_closed_trades.size())
		return nullptr;

	std::shared_ptr<Trade> best = nullptr;

	for(auto trade: m_closed_trades) {
		if(!best || trade->profit() > best->profit())
			best = trade;
	}

	return best;
}

std::shared_ptr<Trade> Simulation::worst_losing_trade() {
	if(!m_closed_trades.size())
		return nullptr;

	std::shared_ptr<Trade> worst = nullptr;

	for(auto trade: m_closed_trades) {
		if(!worst || trade->profit() > worst->profit())
			worst = trade;
	}

	return worst;
}
