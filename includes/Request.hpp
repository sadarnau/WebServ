#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <unistd.h>
# include <string>
# include <iostream>
# include <map>
# include <vector>
# include <sstream>
# include <colors.hpp>
# include <iomanip>
# include "Location.hpp"
# include "Logger.hpp"
# include "Utils.hpp"

class Location ;

typedef std::vector< Location > vlocation;

class Request
{
private:
	vlocation							*_locationVector;
	int									_inSocket;
	std::string							_buff;

	size_t								_contentLength;
	std::string							_method;
	std::string							_target;
	std::string							_message;
	std::string							_body;
	std::string							_urlTargetPath;
	std::string							_absoluteTargetPath;
	std::map<std::string, std::string>	_headers;
	std::vector<std::string>			_skippedHeaders;
	std::string							_queryString;
	Location							_selectedLocation;

	void								_parseRequest(std::string req);
	std::string							_unchunkBody(std::string body);
	void								_parseUrl(void);
	void								_selectLocation(void);
	void								_createPath(void);
	bool								_isValidHeader(std::string header);

public:
	Request(vlocation *_locationVector, int inSock, std::string buff);
	Request(Request const & src);
	~Request(void);
	Request & operator=(Request const & rhs);

	void								updateTarget(std::string target);
	int									getInSock(void);
	std::string							getMethod(void);
	std::string							getBody(void);
	std::string							getTarget(void);
	std::string							getUrlTargetPath(void);
	std::string							getAbsoluteTargetPath(void);
	std::string							getQueryString(void);
	std::map<std::string, std::string>	getHeaders(void);
	Location							getSelectedLocation(void);
	size_t								getContentLength(void);
	
	void								logRequest(int serverNb);
};

std::ostream &	operator<<(std::ostream & o, Request & rhs);

#endif
