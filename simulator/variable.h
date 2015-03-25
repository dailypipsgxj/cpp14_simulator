#ifndef VARIABLE_H
#define VARIABLE_H

#include <random>
#include <string>

#include "lua_script.h"

std::mt19937 new_mt();

class Variable {
protected:
	std::string m_name;
public:
	virtual ~Variable();
	virtual void randomize() = 0;
	static std::shared_ptr<Variable> parse_string(std::string);
	static std::vector<std::shared_ptr<Variable>> load_from_string(std::string);
	std::string get_name();
	virtual std::string show() = 0;
	virtual void send_to_lua(std::shared_ptr<LuaScript>) = 0;
};

class FloatVariable: public Variable {
	float m_value;
	float m_lower;
	float m_upper;
public:
	FloatVariable(std::string, float, float);
	~FloatVariable();
	void randomize();
	std::string show();
	void send_to_lua(std::shared_ptr<LuaScript>);
};

class IntVariable: public Variable {
	int m_value;
	int m_lower;
	int m_upper;
public:
	IntVariable(std::string, int, int);
	~IntVariable();
	void randomize();
	std::string show();
	void send_to_lua(std::shared_ptr<LuaScript>);
};

class BoolVariable: public Variable {
	bool m_value;
public:
	BoolVariable(std::string);
	~BoolVariable();
	void randomize();
	std::string show();
	void send_to_lua(std::shared_ptr<LuaScript>);
};

#endif
