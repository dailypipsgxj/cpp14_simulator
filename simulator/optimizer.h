#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>

#include "lua_script.h"
#include "tick_source.h"
#include "variable.h"

class Score {
	float m_score;
	std::vector<std::shared_ptr<Variable>> m_variables;
public:
	Score(float, std::vector<std::shared_ptr<Variable>>);
	static auto get_best(std::vector<std::shared_ptr<Score>>);
	void print();
	std::vector<std::shared_ptr<Variable>> get_variables();
	float get_score();
};

class Optimizer {
	std::vector<std::shared_ptr<Variable>> m_original_variables;
	std::vector<std::shared_ptr<Score>> m_scores;
public:
	Optimizer(std::vector<std::shared_ptr<Variable>>);
	~Optimizer();
	std::shared_ptr<Score> optimize_variables_on(std::shared_ptr<LuaScript>,
		std::vector<std::shared_ptr<Chart>>,
		std::unique_ptr<TickSource> &);
	void print_scores();
};

#endif
