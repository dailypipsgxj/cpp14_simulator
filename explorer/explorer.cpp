#include <iostream>

#include "chart.h"
#include "lua_script.h"
#include "exploration.h"
#include "tick_source.h"

// ===== FUNCTIONS =================================================================================

int main(int argc, char* argv[]) {
	if(argc != 2) {
		std::cout << "Usage: " << argv[0] << " /path/to/lua/script.lua" << std::endl;
		exit(1);
	}

	auto config = std::make_shared<LuaScript>(argv[1]);
	auto charts = Chart::load_from_string(config->charts());
	auto ts = std::make_unique<TickSource>(config->csv_path());

	ts->fill_charts(charts);
	ts->advance_charts_to_next_sunday(charts);

	Exploration e(config, charts);
	e.run(ts);

	return 0;
}
