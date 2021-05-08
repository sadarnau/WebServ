#ifndef UTILS_HPP
# define UTILS_HPP

# include <time.h>
# include <sys/time.h>
# include <string>
# include <map>
# include <vector>
# include <iterator>
# include <sstream>
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include "Logger.hpp"

class Webserv ;

std::string 	            getTimeHMS(void);
std::string 	            getDate(void);
void 			            splitStringToVector(std::string line, std::vector<std::string> &split);
void			            printMap(std::map<std::string, std::string> m, std::string type);
std::string                 safeUrlJoin(std::string url1, std::string url2);
std::vector<std::string>	concatToVector(std::string toParse);
std::string					intToStr(int i);
bool						isPathAFile(std::string path);
size_t						getChunkLength(std::string body, size_t it);
std::string					getExtension(std::string target);


#endif