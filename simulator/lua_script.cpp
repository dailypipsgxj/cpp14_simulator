#include <cstdio>
#include <iostream>

#include "lua_script.h"

// ===== FUNCTIONS =================================================================================

static int lua_get_nanoseconds(lua_State* L) {
	lua_pushnumber(L, get_nanoseconds());
	return 1;
}

static int lua_noop(lua_State *L) {
	lua_pushnumber(L, 0);
	return 1;
}

static int lua_buy(lua_State *L) {
	lua_pushnumber(L, 1);
	return 1;
}

static int lua_sell(lua_State *L) {
	lua_pushnumber(L, 2);
	return 1;
}

static int lua_close_trade(lua_State *L) {
	lua_pushnumber(L, 3);
	return 1;
}

// ===== LUA SCRIPT ================================================================================

LuaScript::LuaScript(const char *path) {
	std::cout << "Starting Lua interpreter using " << path << std::endl;

	m_state = luaL_newstate();
	if(!m_state) {
		std::cout << "m_state is null" << std::endl;
		bail("failed to initialize Lua");
	}

	luaL_openlibs(m_state);

	lua_pushcfunction(m_state, lua_get_nanoseconds);
	lua_setglobal(m_state, "get_nanoseconds");

	lua_pushcfunction(m_state, lua_noop);
	lua_setglobal(m_state, "noop");

	lua_pushcfunction(m_state, lua_buy);
	lua_setglobal(m_state, "buy");

	lua_pushcfunction(m_state, lua_sell);
	lua_setglobal(m_state, "sell");

	lua_pushcfunction(m_state, lua_close_trade);
	lua_setglobal(m_state, "close_trade");

	// seed PRNG with nanoseconds (TODO: does not work apparently lol)
	luaL_dostring(m_state, "math.randomseed(get_nanoseconds)");

	if(luaL_loadfile(m_state, path)) {
		bail("luaL_loadfile() failed");
	}

	if(lua_pcall(m_state, 0, 0, 0)) {
		bail("lua_pcall() failed");
	}
}

LuaScript::~LuaScript() {
	std::cout << "Closing Lua interpeter" << std::endl;

	lua_close(m_state);
	m_state = nullptr;
}

TradingDecision LuaScript::on_tick() {
	lua_getglobal(m_state, "on_tick");
	if (lua_pcall(m_state, 0, 1, 0)) {
		bail("lua_pcall() on_tick failed");
	}

	if(!lua_isnumber(m_state, -1)) {
		bail("expected numeric return value from on_tick()");
	}

	int retval = static_cast<int>(lua_tonumber(m_state, -1));
	lua_pop(m_state, 1);

	switch(retval) {
		case 0:
			return NOOP;
		case 1:
			return BUY;
		case 2:
			return SELL;
		case 3:
			return CLOSE;
		default:
			std::cout << "Value: " << retval << std::endl;
			bail("Received unknown value from on_tick()");
	}
}

void LuaScript::buy_signal() {
	lua_getglobal(m_state, "buy_signal");
	if (lua_pcall(m_state, 0, 0, 0)) {
		bail("lua_pcall() on buy_signal() failed");
	}
}

void LuaScript::sell_signal() {
	lua_getglobal(m_state, "sell_signal");
	if (lua_pcall(m_state, 0, 0, 0)) {
		bail("lua_pcall() on sell_signal() failed");
	}
}

void LuaScript::before_run() {
	lua_getglobal(m_state, "before_run");
	if (lua_pcall(m_state, 0, 0, 0)) {
		bail("lua_pcall() on before_run() failed");
	}
}

void LuaScript::after_run() {
	lua_getglobal(m_state, "after_run");
	if (lua_pcall(m_state, 0, 0, 0)) {
		bail("lua_pcall() on after_run() failed");
	}
}

// ----- LUA FUNCTIONS -----------------------------------------------------------------------------

void LuaScript::set_string(std::string name, std::string value) {
	lua_pushstring(m_state, value.c_str());
	lua_setglobal(m_state, name.c_str());
}

void LuaScript::set_bool(std::string name, bool value) {
	lua_pushboolean(m_state, value);
	lua_setglobal(m_state, name.c_str());
}

void LuaScript::set_number(std::string name, int value) {
	lua_pushnumber(m_state, value);
	lua_setglobal(m_state, name.c_str());
}

void LuaScript::set_number(std::string name, float number) {
	lua_pushnumber(m_state, number);
	lua_setglobal(m_state, name.c_str());
}

// ----- LUA TABLE FUNCTIONS -----------------------------------------------------------------------

void LuaScript::create_table(int size) {
	lua_createtable(m_state, size, 0);
}

void LuaScript::create_table_candle(int index,
				float open_bid,
				float open_ask,
				float close_bid,
				float close_ask,
				int volume) {
	lua_pushinteger(m_state, index);
	lua_createtable(m_state, 5, 0);

	lua_pushstring(m_state, "open_bid");
	lua_pushnumber(m_state, open_bid);
	lua_rawset(m_state, -3);

	lua_pushstring(m_state, "open_ask");
	lua_pushnumber(m_state, open_ask);
	lua_rawset(m_state, -3);

	lua_pushstring(m_state, "close_bid");
	lua_pushnumber(m_state, close_bid);
	lua_rawset(m_state, -3);

	lua_pushstring(m_state, "close_ask");
	lua_pushnumber(m_state, close_ask);
	lua_rawset(m_state, -3);

	lua_pushstring(m_state, "volume");
	lua_pushinteger(m_state, volume);
	lua_rawset(m_state, -3);

	lua_rawset(m_state, -3);
}

void LuaScript::finalize_table(std::string name) {
	lua_setglobal(m_state, name.c_str());
}

// ----- LUA VARIABLES -----------------------------------------------------------------------------

std::string LuaScript::charts() {
	return get_string_var("CHARTS");
}

std::string LuaScript::csv_path() {
	return get_string_var("CSV_PATH");
}

std::string LuaScript::in_sample_time() {
	return get_string_var("IN_SAMPLE_TIME");
}

int LuaScript::optimizations() {
	return get_int_var("OPTIMIZATIONS");
}

int LuaScript::steps() {
	return get_int_var("STEPS");
}

std::string LuaScript::out_of_sample_time() {
	return get_string_var("OUT_OF_SAMPLE_TIME");
}

std::string LuaScript::post_run_script() {
	return get_string_var("POST_RUN_SCRIPT");
}

std::string LuaScript::variables() {
	return get_string_var("VARIABLES");
}

float LuaScript::initial_deposit() {
	return get_float_var("INITIAL_DEPOSIT");
}

float LuaScript::minimum_optimization_score() {
	return get_float_var("MINIMUM_OPTIMIZATION_SCORE");
}

float LuaScript::minimum_execution_score() {
	return get_float_var("MINIMUM_EXECUTION_SCORE");
}

float LuaScript::minimum_overall_score() {
	return get_float_var("MINIMUM_OVERALL_SCORE");
}

float LuaScript::drawdown_limit() {
	return get_float_var("DRAWDOWN_LIMIT");
}

float LuaScript::get_float_var(std::string name) {
	lua_getglobal(m_state, name.c_str());

	if(!lua_isnumber(m_state, -1)) {
		char err[255];
		sprintf(err, "'%s' should be a valid integer name", name.c_str());
		bail(err);
	}

	return static_cast<float>(lua_tonumber(m_state, -1));
}

int LuaScript::get_int_var(std::string name) {
	lua_getglobal(m_state, name.c_str());

	if(!lua_isnumber(m_state, -1)) {
		char err[255];
		sprintf(err, "'%s' should be a valid integer name", name.c_str());
		bail(err);
	}

	return static_cast<int>(lua_tonumber(m_state, -1));
}

std::string LuaScript::get_string_var(std::string name) {
	lua_getglobal(m_state, name.c_str());

	if(!lua_isstring(m_state, -1)) {
		char err[255];
		sprintf(err, "'%s' should be a valid std::string name", name.c_str());
		bail(err);
	}

	return static_cast<std::string>(lua_tostring(m_state, -1));
}
