#include "utils.h"

#include <iostream>

#include <numeric>

#include <time.h>
#include <sys/time.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include <boost/algorithm/string.hpp>

float sum_vector(std::vector<float> vec) {
	return std::accumulate(vec.begin(), vec.end(), static_cast<float>(0.0));
}

[[ noreturn ]] void bail(const char *msg) {
	std::cout << msg << std::endl;
	exit(1);
}

int get_nanoseconds() {
	struct timespec ts;

#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts.tv_sec = mts.tv_sec;
	ts.tv_nsec = mts.tv_nsec;
#else
	clock_gettime(CLOCK_REALTIME, &ts);
#endif

	return static_cast<int>(ts.tv_nsec);
}

void trim_strings(std::vector<std::string> &strs) {
	// can't use the new fancy iterator here because it makes a copy
	for(unsigned long i = 0; i < strs.size(); i++) {
		boost::trim(strs[i]);
	}
}
