#include "tick_source.h"

#include <iostream>
#include <iomanip>
#include <fstream>

#include <boost/algorithm/string.hpp>

const unsigned long RECORD_SIZE = 16; // sizeof(time_t) + sizeof(float) + sizeof(float)

TickSource::TickSource(std::string path) {
	std::cout << "Opening " << path << " as ticksource" << std::endl;

	m_records_read = 0;

	std::ifstream input(path, std::ios::in | std::ios::binary);
	if(!input.is_open()) {
		bail("Failed to open CSV file");
	}

	input.seekg(0, input.end);
	m_data_size = static_cast<unsigned long>(input.tellg());
	input.seekg(0);

	m_data = new char[m_data_size];

	input.read(m_data, static_cast<long>(m_data_size));
	if (!input) {
		bail("failed to read file into buffer");
	}

	show_initialization_summary();
}

TickSource::TickSource(char *data, unsigned long size) {
	std::cout << "Creating ticksource from subset of existing tick source ("
		<< size << " bytes requested)"<< std::endl;

	m_data = data;

	m_records_read = 0;
	m_data_size = size;

	show_initialization_summary();
}

TickSource::~TickSource() {
	std::cout << "Closing ticksource (read " << m_records_read * RECORD_SIZE << " bytes, "
		<< m_records_read << " records)" << std::endl;

	delete[] m_data;
}

std::unique_ptr<TickSource> TickSource::subset(unsigned long start_record, unsigned long end_record) {
	if(start_record > end_record) {
		std::cout << "Start: " << start_record << ", End: " << end_record << std::endl;
		bail("end_record must be >= start_record");
	}

	char *start = (m_data + (start_record * RECORD_SIZE));
	// add an extra one here so it's inclusive of end_record
	unsigned long size = (end_record - start_record + 1) * RECORD_SIZE;

	char *new_data = new char[size];
	memcpy(new_data, start, size);

	return std::make_unique<TickSource>(new_data, size);
}

std::shared_ptr<Tick> TickSource::next() {
	unsigned long offset = m_records_read * RECORD_SIZE;

	if(offset > (m_data_size - RECORD_SIZE)) {
		return nullptr;
	} else {
		time_t *time = reinterpret_cast<time_t *>(m_data + offset);
		float *bid   = reinterpret_cast<float *>(m_data + offset + 8);
		float *ask   = reinterpret_cast<float *>(m_data + offset + 12);

		m_records_read++;

		return std::make_shared<Tick>(*time, *bid, *ask);
	}
}

std::shared_ptr<Tick> TickSource::peek_next() {
	return peek_at(next_record_id());
}

void TickSource::step_back() {
	m_records_read--;
}

void TickSource::seek_to_record(unsigned long id) {
	if(id * RECORD_SIZE >= m_data_size) {
		std::cout << "Record id: " << id << std::endl;
		bail("bad record id");
	}

	m_records_read = id;
}

unsigned long TickSource::next_record_id() {
	return m_records_read;
}

std::shared_ptr<Tick> TickSource::peek_at(unsigned long id) {
	auto old_id = next_record_id();

	seek_to_record(id);
	auto tick = next();
	seek_to_record(old_id);

	return tick;
}

void TickSource::show_initialization_summary() {
	unsigned long num_records = m_data_size / RECORD_SIZE;

	std::cout << "Read " << num_records << " records (" << m_data_size
		<< " bytes) into buffer" << std::endl;

	std::cout << "First tick: " << next()->show() << std::endl;
	seek_to_record(num_records - 1);
	std::cout << "Last tick: " << next()->show() << std::endl;
	seek_to_record(0);
}


void TickSource::advance_charts_to_next_sunday(std::vector<std::shared_ptr<Chart>> charts) {
	bool tick_is_sunday = true;
	bool on_sunday = true;
	int count = 0;

	std::cout << "==================== ADVANCING TO NEXT SUNDAY ====================" << std::endl;

	std::cout << "Starting at: " << peek_next()->show() << std::endl;

	for(auto tick = next(); tick; tick = next()) {
		tick_is_sunday = tick->is_sunday();

		if(!on_sunday && tick_is_sunday) {
			on_sunday = true;
			break;
		} else if(on_sunday && !tick_is_sunday) {
			on_sunday = false;
		}

		for(auto chart: charts) {
			chart->process_tick(tick);
		}

		count++;
	}

	step_back();

	if(!on_sunday) {
		std::cout << "Final tick: " << peek_next()->show() << std::endl;
		bail("ended on a non-sunday day");
	}

	std::cout << "Consumed " << count << " ticks" << std::endl;
	std::cout << "Ended at tick before: " << peek_next()->show() << std::endl;
}

void TickSource::fill_charts(std::vector<std::shared_ptr<Chart>> charts) {
	bool all_charts_have_data = true;
	int count = 0;

	std::cout << "==================== FILLING CHARTS ====================" << std::endl;

	for(auto tick = next(); tick; tick = next()) {
		count++;

		for(auto chart: charts) {
			chart->process_tick(tick);
		}

		all_charts_have_data = true;

		for(auto chart: charts) {
			if(!chart->has_full_data()) {
				all_charts_have_data = false;
				break;
			}
		}

		if(all_charts_have_data) {
			std::cout << "Stopping on tick before: " << tick->show() << std::endl;
			break;
		}
	}

	step_back();

	std::cout << "Consumed " << count - 1 << " ticks" << std::endl;
}
