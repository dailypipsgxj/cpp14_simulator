#ifndef TICK_H
#define TICK_H

#include <ctime>
#include <time.h>
#include <string>

#include "utils.h"

enum DayOfWeek : int {
	SUNDAY = 0,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
};

class Tick {
	std::string m_line;
	float m_bid, m_ask;
	time_t m_time;
	struct tm m_tm;

public:
	Tick(time_t, float, float);
	float get_bid();
	float get_ask();
	time_t get_time();
	DayOfWeek get_day_of_week();
	bool is_sunday();
	std::string show();

	// TODO: CSV functions
};

#endif
