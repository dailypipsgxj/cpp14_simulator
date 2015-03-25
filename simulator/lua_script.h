#ifndef LUA_SCRIPT_H
#define LUA_SCRIPT_H

#include "utils.h"

// #include "lua.hpp" /* TODO: allegedly needed for LuaJIT? */
extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>  /* Prototype for luaL_openlibs(), */
}

enum TradingDecision : int {
	NOOP = 0,
	BUY,
	SELL,
	CLOSE
};

class LuaScript {
	lua_State *m_state;

public:
	LuaScript(const char *);
	~LuaScript();
	TradingDecision on_tick();
	void buy_signal();
	void sell_signal();
	void before_run();
	void after_run();

	// ----- LUA FUNCTIONS ---------------------------------------------------------------------

	void set_string(std::string, std::string);
	void set_bool(std::string, bool);
	void set_number(std::string, float);
	void set_number(std::string, int);

	// ----- LUA TABLE FUNCTIONS ---------------------------------------------------------------

	void create_table(int size);
	void create_table_candle(int, float, float, float, float, int);
	void finalize_table(std::string);

	// ----- LUA VARIABLES ---------------------------------------------------------------------

	std::string charts();
	std::string csv_path();
	std::string in_sample_time();
	int optimizations();
	int steps();
	float initial_deposit();
	float minimum_optimization_score();
	float minimum_execution_score();
	float minimum_overall_score();
	float drawdown_limit();
	std::string out_of_sample_time();
	std::string post_run_script();
	std::string variables();

protected:
	float get_float_var(std::string);
	int get_int_var(std::string);
	std::string get_string_var(std::string);
};

#endif
