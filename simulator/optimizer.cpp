#include <iostream>
#include <random>

#include "chart.h"
#include "optimizer.h"
#include "simulation.h"
#include "utils.h"

// ===== SCORE =====================================================================================

Score::Score(float score, std::vector<std::shared_ptr<Variable>> variables) {
	m_score = score;
	m_variables = variables;
}

auto Score::get_best(std::vector<std::shared_ptr<Score>> scores) {
	if(0 == scores.size()) {
		bail("scores must have at least one element");
	}

	auto highest_score = *scores.begin();

	for(auto score: scores) {
		if(score->get_score() > highest_score->get_score()) {
			highest_score = score;
		}
	}

	return highest_score;
}

void Score::print() {
	std::cout << "Score: " << m_score << std::endl;
	std::cout << "Variables: " << std::endl;
	for(auto var: m_variables) {
		std::cout << "\t" << var->get_name() << " -> " << var->show() << std::endl;
	}
	std::cout << std::endl;
}

std::vector<std::shared_ptr<Variable>> Score::get_variables() {
	return m_variables;
}

float Score::get_score() {
	return m_score;
}

// ===== OPTIMIZER =================================================================================

Optimizer::Optimizer(std::vector<std::shared_ptr<Variable>> variables) {
	m_original_variables = variables;

	std::cout << "Optimizer loaded with variables" << std::endl;
}

Optimizer::~Optimizer() {
	std::cout << "Optimizer destroyed" << std::endl;
}

std::shared_ptr<Score> Optimizer::optimize_variables_on(
		std::shared_ptr<LuaScript> config,
		std::vector<std::shared_ptr<Chart>> charts,
		std::unique_ptr<TickSource> &ts
	) {

	std::cout << "==================== RUNNING OPTIMIZER ====================" << std::endl;

	// TODO: run these in a config-defined number of concurrent threads
	for(auto i = 0; i < config->optimizations(); i++) {
		auto new_vars = m_original_variables;
		auto new_config = config;
		auto new_charts = charts;

		for(auto var: new_vars) {
			var->randomize();
			var->send_to_lua(new_config);
		}

		auto original_record_id = ts->next_record_id();

		Simulation sim(new_config, new_charts, new_vars);
		sim.run(ts);

		ts->seek_to_record(original_record_id);

		auto score = std::make_shared<Score>(sim.get_score(), new_vars);
		m_scores.push_back(score);
	}

	return Score::get_best(m_scores);
}

void Optimizer::print_scores() {
	for(auto score: m_scores) {
		score->print();
	}
}
