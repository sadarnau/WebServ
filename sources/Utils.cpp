#include "Utils.hpp"

std::string 	getTimeHMS(void)
{
    struct timeval 	tv;
    struct tm 		time;
    struct timezone tz;
    char 			buffer[1000];
    std::string 	date;

    gettimeofday(&tv, &tz);
	std::ostringstream ss;
	ss << tv.tv_sec;
	std::string res = ss.str();
    strptime(res.c_str(), "%s", &time);
    strftime(buffer, sizeof(buffer), "%H:%M:%S", &time);
    date = buffer;
	return (date);
}