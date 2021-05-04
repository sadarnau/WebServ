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

void		printLocation(Location loc)
{
	std::ostringstream oss;
	std::vector<std::string> vec;
	std::map<std::string, std::string> mapstr;

	oss << "location ";
	oss << "[path: " << loc.getPath() << "] : ";
	oss << "[listen: " << loc.getListen() << "]";
	oss << "[server_name: " << loc.getServerName() << "]";
	oss << "[client_max_body_size: " << loc.getClientMaxBodySize() << "]";
	oss << "[root: " << loc.getRoot() << "]";
	oss << "[cgi: " << loc.getCgi() << "]";
	oss << "[autoindex: " << loc.getAutoindex() << "]";
	oss << "[index: ";
	vec = loc.getIndex();
	if (!vec.empty())
	{
		for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it)
			oss << *it << " ";
	}
	oss << "[accepted_method: ";
	vec = loc.getAcceptedMethod();
	if (!vec.empty())
	{
		for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); ++it)
			oss << *it << " ";
	}
	oss << "]";

	mapstr = loc.getErrorPage();

	oss << "[error_pages :";
	if (!mapstr.empty())
	{
		for (std::map<std::string, std::string>::const_iterator it = mapstr.begin(); it != mapstr.end(); ++it)
			oss << it->first << " " << it->second << "\n";
	}
	oss << "]";

	Logger::Write(Logger::MORE, WHT, oss.str());
}

void			printServer(Webserv wserv)
{
	std::vector<Location> locVector;
	std::ostringstream oss;

	locVector = wserv.getLocationVector();
	oss << wserv;
	Logger::Write(Logger::DEBUG, WHT, oss.str());

	for (std::vector<class Location>::const_iterator it2 = locVector.begin(); it2 != locVector.end(); ++it2)
		printLocation(*it2);
	oss.str("");
	oss.clear();
}

void			printAllServers(std::vector<class Webserv> serverList)
{
	std::ostringstream oss;
	std::vector<Location> locVector;
	Location loc;
	Webserv wserv;

	for (std::vector<class Webserv>::const_iterator it = serverList.begin(); it != serverList.end(); ++it)
		printServer(*it);
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
	std::ostringstream convert;   // stream used for the conversion
	convert << i;      // insert the textual representation of 'Number' in the characters in the stream
	return (convert.str());
}