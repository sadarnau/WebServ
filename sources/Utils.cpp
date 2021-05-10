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

bool			isPathAFile(std::string path)
{
	struct stat s;

	if (stat(path.c_str(), &s) == 0 )
	{
		if (s.st_mode & S_IFREG)
			return (true);
	}
	return (false);
}

std::string 	getDate(void)
{
    struct timeval 	tv;
    struct tm 		time;
    struct timezone tz;
    char 			buffer[1000];
    std::string 	date;

    gettimeofday(&tv, &tz);
    strptime(std::to_string(tv.tv_sec).c_str(), "%s", &time);
    strftime(buffer, sizeof(buffer), "%a, %d %b %Y %H:%M:%S CEST", &time);
    date = buffer;
	return (date);
}

void 			splitStringToVector(std::string line, std::vector<std::string> &split)
{
	std::string res;
	size_t pos1 = 0;
	size_t pos2 = 0;
	char *str = new char [line.length() + 1];

	std::strcpy(str, line.c_str());
	str[line.length()] = 0;
	while (str[pos1] != 0 && str[pos2] != 0)
	{
		while ((str[pos1] == ' ' || str[pos1] == '\t') && str[pos1] != 0)
			++pos1;
		if (str[pos1] == 0)
			break ;
		pos2 = pos1 + 1;
		while ((str[pos2] != ' ' && str[pos2] != '\t') && str[pos2] != 0)
			++pos2;
		res = line.substr(pos1, pos2 - pos1);
		split.push_back(res);
		pos1 = pos2;
	}
	delete[] str;
}

void			printMap(std::map<std::string, std::string> m, std::string type)
{
	std::ostringstream oss;

	oss << type;
	for (std::map<std::string, std::string>::const_iterator it = m.begin(); it != m.end(); ++it)
	{
		oss << std::setw(20) << std::left << it->first << ":" << it->second << "\r\n";
	}
	oss << '\n' << "----------\n\n";
	Logger::Write(Logger::DEBUG, WHT, oss.str());
}

std::string	safeUrlJoin(std::string url1, std::string url2)
{

	if(url1.back() == '/' )
		url1 = url1.substr(0, url1.size() - 1);

	if(url2.front() == '/')
		url2 = url2.substr(1, url2.size());

	return (std::string(url1 + "/" + url2));
}

std::vector<std::string>	concatToVector(std::string toParse)
{
	std::vector<std::string> result;
	std::string separator = "/";
	std::string token;
	size_t pos;

	while ((pos = toParse.find(separator)) != std::string::npos)
	{
		token = toParse.substr(0, pos);
		toParse = toParse.substr(pos + 1, toParse.length());
		result.push_back(token);
	}
	result.push_back(toParse);

	return (result);
}

std::string					intToStr(int i)
{
	std::ostringstream convert;
	convert << i;
	return (convert.str());
}

size_t						getChunkLength(std::string body, size_t it)
{
	size_t 				length;   
	std::stringstream 	ss;

	ss << std::hex << body.substr(0, it);
	ss >> length;
	return (length);
}

std::string					getExtension(std::string target)
{
	if(target.find(".") == std::string::npos)
		return ("");
		
	std::string ext = target.substr(target.find("."), target.length());
	return ext;
}