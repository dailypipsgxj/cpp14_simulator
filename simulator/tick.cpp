#include "tick.h"

Tick::Tick(time_t time, float bid, float ask) {
	struct tm *tm = ::localtime(&time);

	m_time = time;
	m_bid = bid;
	m_ask = ask;
	m_tm = *tm;
}

float Tick::get_bid() {
	return m_bid;
}

float Tick::get_ask() {
	return m_ask;
}

time_t Tick::get_time() {
	return m_time;
}

DayOfWeek Tick::get_day_of_week() {
	switch(m_tm.tm_wday) {
		case 0:  return SUNDAY;
		case 1:  return MONDAY;
		case 2:  return TUESDAY;
		case 3:  return WEDNESDAY;
		case 4:  return THURSDAY;
		case 5:  return FRIDAY;
		default: return SATURDAY;
	}
}

bool Tick::is_sunday() {
	return SUNDAY == get_day_of_week();
}

std::string Tick::show() {
	char buffer[80];
	strftime(buffer, 80, "%Y-%m-%d %X", &m_tm);
	std::string str(buffer);
	return str;
}
