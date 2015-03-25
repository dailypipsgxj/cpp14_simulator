#include <cmath>
#include <iostream>

#include "chart.h"
#include "lua_script.h"
#include "optimizer.h"
#include "simulation.h"
#include "tick.h"
#include "tick_source.h"
#include "utils.h"
#include "variable.h"

// ===== FUNCTIONS =================================================================================

int main(int argc, char* argv[]) {
	if(argc != 2) {
		std::cout << "Usage: " << argv[0] << " /path/to/lua/script.lua" << std::endl;
		exit(1);
	}

	std::cout.unsetf(std::ios::floatfield);
	std::cout.precision(5);
	std::cout << std::fixed;

	std::cout << "==================== SETUP ====================" << std::endl;

	// TODO: detect unusual currency bases for accurate profit calculation

	auto config = std::make_shared<LuaScript>(argv[1]);

	std::cout << "Simulating a maximum of " << config->steps() << " steps" << std::endl;
	std::cout << config->optimizations() << " optimizations per in sample" << std::endl;

	auto charts = Chart::load_from_string(config->charts());
	auto ts = std::make_unique<TickSource>(config->csv_path());
	auto vars = Variable::load_from_string(config->variables());
	auto optimizer = std::make_unique<Optimizer>(vars);

	ts->fill_charts(charts);
	ts->advance_charts_to_next_sunday(charts);

	int step_number = 1;
	int weeks_in_sample = std::stoi(config->in_sample_time());
	int weeks_out_of_sample = std::stoi(config->out_of_sample_time());

	std::vector<float> optimization_scores;
	std::vector<float> execution_scores;

	// ----- MAIN LOOP -------------------------------------------------------------------------

	for(;;) {
		// ----- FIND IN AND OUT OF SAMPLE START AND END POINTS ----------------------------

		// in_sample_record_end is just out_of_sample_record_start - 1
		unsigned long in_sample_record_start = ts->next_record_id();
		unsigned long out_of_sample_record_start = 0;
		unsigned long out_of_sample_record_end = 0;

		bool tick_is_sunday = true;
		bool on_sunday = true;
		int sundays_seen = 0;

		for(auto tick = ts->next(); tick; tick = ts->next()) {
			tick_is_sunday = tick->is_sunday();

			if(!on_sunday && tick_is_sunday) {
				on_sunday = true;
				sundays_seen++;
			} else if(on_sunday && !tick_is_sunday) {
				on_sunday = false;
			}

			if(sundays_seen == weeks_in_sample) {
				out_of_sample_record_start = ts->next_record_id() - 1;
				break;
			}
		}

		if(!out_of_sample_record_start) {
			bail("ran out of tick source when searching for out of sample start");
		}

		sundays_seen = 0;
		on_sunday = true;

		for(auto tick = ts->next(); tick; tick = ts->next()) {
			tick_is_sunday = tick->is_sunday();

			if(!on_sunday && tick_is_sunday) {
				on_sunday = true;
				sundays_seen++;
			} else if(on_sunday && !tick_is_sunday) {
				on_sunday = false;
			}

			if(sundays_seen == weeks_out_of_sample) {
				// stop on the tick before the current tick
				out_of_sample_record_end = ts->next_record_id() - 2;
				break;
			}
		}

		if(!out_of_sample_record_end) {
			bail("ran out of tick source when searching for out of sample end");
		}

		std::cout << "In sample: " << ts->peek_at(in_sample_record_start)->show()
			<< " through " << ts->peek_at(out_of_sample_record_start - 1)->show()
			<< std::endl;

		std::cout << "Out of sample: " << ts->peek_at(out_of_sample_record_start)->show()
			<< " through " << ts->peek_at(out_of_sample_record_end)->show()
			<< std::endl;

		// ----- OPTIMIZE ON IN SAMPLE PERIOD ----------------------------------------------

		auto subset = ts->subset(in_sample_record_start, out_of_sample_record_start - 1);

		auto winner = optimizer->optimize_variables_on(config, charts, subset);
		// optimizer->print_scores();

		// ----- IF OPTIMIZATION SUCCEEDED, EXECUTE ON OUT OF SAMPLE PERIOD ----------------

		if(winner->get_score() < config->minimum_optimization_score()) {
			bail("Failed to find a variables above the cutoff");
		}

		optimization_scores.push_back(winner->get_score());

		std::cout << "==================== EXECUTING ON ====================" << std::endl;
		std::cout << "Winning variables!" << std::endl;
		std::cout << "Score: " << winner->get_score() << std::endl;
		for(auto var: winner->get_variables()) {
			std::cout << "\t" << var->get_name() << " -> " << var->show() << std::endl;
		}

		auto new_vars = winner->get_variables();
		auto new_config = config;
		auto new_charts = charts;

		auto oos = ts->subset(out_of_sample_record_start, out_of_sample_record_end);

		Simulation sim(new_config, new_charts, new_vars);
		sim.run(oos);

		std::cout << "Execution score: " << sim.get_score() << std::endl;
		std::cout << "% profitable: " << sim.percentage_of_profitable_trades()
			<< "%" << std::endl;
		std::cout << "Average trade duration: " << sim.average_trade_duration() << std::endl;
		std::cout << "Worst DD: " << sim.get_worst_drawdown() << "%" << std::endl;
		std::cout << "Equity high: " << sim.get_equity_high() << std::endl;
		std::cout << "Equity low: " << sim.get_equity_low() << std::endl;
		std::cout << "Trades: " << sim.get_trade_count() << std::endl;
		std::cout << "Winners: " << sim.get_winning_trade_count() << std::endl;
		std::cout << "Losers: " << sim.get_losing_trade_count() << std::endl;
		std::cout << "Best: " << sim.best_winning_trade()->profit() << std::endl;
		std::cout << "Worst: " << sim.worst_losing_trade()->profit() << std::endl;

		// ----- IF EXECUTION SUCCEEDED, RECORD RESULTS ------------------------------------

		if(sim.get_score() < config->minimum_execution_score()) {
			bail("Failed at optimization step");
		}

		execution_scores.push_back(sim.get_score());

		// ----- EXIT IF WE'RE AT MAX STEPS ------------------------------------------------

		step_number++;

		if(step_number > config->steps()) {
			std::cout << "Reached max steps, breaking" << std::endl;
			break;
		}

		// ----- ADVANCE CHARTS TO NEXT IN SAMPLE START ------------------------------------

		ts->seek_to_record(in_sample_record_start);
		ts->advance_charts_to_next_sunday(charts);
	}

	std::cout << "==================== RESULTS ====================" << std::endl;

	// ----- SHOW PROFIT FOR EACH PERIOD -------------------------------------------------------

	std::cout << "Scores:" << std::endl;

	for(unsigned long i = 0; i < optimization_scores.size(); i++) {
		std::cout << i + 1 << " - ";
		std::cout << "Opti: " << optimization_scores[i];
		std::cout << " - ";
		std::cout << "Exec: " << execution_scores[i];
		std::cout << std::endl;
	}

	// ----- SHOW WALK FORWARD EFFICIENCY ------------------------------------------------------

	float optimization_total = sum_vector(optimization_scores);
	float execution_total = sum_vector(execution_scores);
	float efficiency = execution_total / optimization_total;

	std::cout << "Optimization total: " << optimization_total << " - ";
	std::cout << "Execution total: " << execution_total << " - ";
	std::cout << "WFA efficiency: " << efficiency << "% - Verdict: ";

	if(efficiency < 0.5) {
		std::cout << "FAIL";
	} else if(efficiency >= 0.5 && efficiency < 1.0) {
		std::cout << "ACCEPTABLE";
	} else if(efficiency >= 1.0 && efficiency <= 1.5) {
		std::cout << "GREAT";
	} else {
		std::cout << "TOO GOOD - SUSPICIOUS";
	}

	std::cout << std::endl;

	// ----- SHOW PROFIT FACTOR ----------------------------------------------------------------

	float gross_profit = 0.0;
	float gross_loss   = 0.0;

	for(auto score: execution_scores) {
		if(score >= 0.0)
			gross_profit += score;
		else
			gross_loss += score;
	}

	gross_loss = std::abs(gross_loss);

	float profit_factor = gross_profit / gross_loss;

	std::cout << "Gross profit: " << gross_profit << " - ";
	std::cout << "Gross loss: " << gross_loss << " - ";
	std::cout << "Profit factor: " << profit_factor << " - Verdict: ";

	if(profit_factor < 1.5) {
		std::cout << "FAIL";
	} else if(profit_factor >= 1.5 && profit_factor < 2.0) {
		std::cout << "ACCEPTABLE";
	} else if(profit_factor >= 2.0 && profit_factor <= 3.0) {
		std::cout << "GREAT";
	} else {
		std::cout << "TOO GOOD - SUSPICIOUS";
	}

	std::cout << std::endl;

	// ----- SHOW FINAL SCORE ------------------------------------------------------------------

	// TODO: final score should be some function of two or more of the following:
	//       execution_total, WFA efficiency, max drawdown, profit factor, % profitable

	std::cout << "Final score: " << execution_total << " - Minimum score: "
		<< config->minimum_overall_score() << " - Verdict: ";

	std::cout << (execution_total >= config->minimum_overall_score() ? "PASS" : "FAIL") << std::endl;

	std::cout << "==================== SHUTDOWN ====================" << std::endl;

	return 0;
}
