#ifndef CANDLESTICK_H
#define CANDLESTICK_H

#include "../chart.h"
#include "../tick.h"

class CandlestickChart: public Chart
{
public:
	CandlestickChart(std::string a, std::string b, unsigned long c) : Chart(a, b, c) {}
	~CandlestickChart();
	void process_tick(std::shared_ptr<Tick>);
	void send_to_lua();
};

#endif
