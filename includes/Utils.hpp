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
# include "Webserv.hpp"

class Webserv ;

std::string 	            getTimeHMS(void);
void 			            splitStringToVector(std::string line, std::vector<std::string> &split);
void			            printMap(std::map<std::string, std::string> m, std::string type);
void			            printAllServers(std::vector<Webserv> serverList);
std::string                 safeUrlJoin(std::string url1, std::string url2);
std::vector<std::string>	concatToVecor(std::string toParse);

#endif