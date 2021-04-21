#ifndef UTILS_HPP
# define UTILS_HPP

# include <time.h>
# include <sys/time.h>
# include <string>
# include <map>
# include <vector>
# include <iterator>
# include <sstream>
# include "Logger.hpp"

std::string 	getTimeHMS(void);
void 			splitStringToVector(std::string line, std::vector<std::string> &split);
void			printMap(std::map<std::string, std::string> m);
void			printLocationMap(std::map<std::string, std::map<std::string, std::string> > m);

#endif