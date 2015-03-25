#ifndef TICK_SOURCE_H
#define TICK_SOURCE_H

#include <string>
#include <vector>

#include "chart.h"
#include "tick.h"

class TickSource {
	char *m_data;
	unsigned long m_data_size;
	std::string m_line;
	unsigned long m_records_read;

public:
	TickSource(std::string);
	TickSource(char *, unsigned long);
	~TickSource();
	std::shared_ptr<Tick> next();
	std::shared_ptr<Tick> peek_next();
	void step_back();
	void seek_to_record(unsigned long);
	unsigned long next_record_id();
	std::shared_ptr<Tick> peek_at(unsigned long);
	void show_initialization_summary();
	std::unique_ptr<TickSource> subset(unsigned long, unsigned long);
	void advance_charts_to_next_sunday(std::vector<std::shared_ptr<Chart>>);
	void fill_charts(std::vector<std::shared_ptr<Chart>>);
};

#endif
