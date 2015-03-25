#ifndef SIMULATION_H
#define SIMULATION_H

#include "chart.h"
#include "lua_script.h"
#include "tick.h"
#include "tick_source.h"
#include "trade.h"
#include "variable.h"

class Simulation {
	float m_score;
	std::shared_ptr<LuaScript> m_script;
	std::vector<std::shared_ptr<Chart>> m_charts;
	std::vector<std::shared_ptr<Variable>> m_variables;

	std::vector<std::shared_ptr<Trade>> m_open_trades;
	std::vector<std::shared_ptr<Trade>> m_closed_trades;

	unsigned int m_total_trade_count;

	float m_deposit;
	float m_last_equity_high;
	float m_last_equity_low;
	float m_equity_high;
	float m_equity_low;
	float m_worst_drawdown;
	float m_drawdown_limit;
public:
	Simulation(std::shared_ptr<LuaScript>,
		std::vector<std::shared_ptr<Chart>>,
		std::vector<std::shared_ptr<Variable>>);
	~Simulation();
	void run(std::unique_ptr<TickSource> &);
	float get_score();
	void open_long_trade(std::shared_ptr<Tick>);
	void open_short_trade(std::shared_ptr<Tick>);
	void close_all_trades(std::shared_ptr<Tick>);

	float get_balance(); // "balance" is the sum of deposit + closed trades
	float get_equity(); // "equity" is the sum of current balance + profit/loss of open trades
	float get_profit(); // balance - deposit
	float expectancy_per_trade(); // profit() / number of closed trades
	float percentage_of_profitable_trades();
	float average_trade_duration();

	float get_equity_high();
	float get_equity_low();
	float get_worst_drawdown();
	bool has_exceeded_max_drawdown();
	void update_stats();

	unsigned long get_trade_count();
	unsigned long get_winning_trade_count();
	unsigned long get_losing_trade_count();

	std::shared_ptr<Trade> best_winning_trade();
	std::shared_ptr<Trade> worst_losing_trade();
};

#endif
