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
# include "Logger.hpp"
# include "Utils.hpp"
# include "Location.hpp"

typedef std::vector< Location > vlocation;

class Request
{
private:

	vlocation							*_locationVector;
	int									_inSocket;
	std::string							_buff;

	std::string							_method;
	std::string							_target;
	std::string							_urlTargetPath;
	std::string							_absoluteTargetPath;
	std::map<std::string, std::string>	_headers;
	std::vector<std::string>			_skippedHeaders;
	std::string							_queryString;
	std::map<std::string, std::string>	_selectedLocation;

public:

	Request( void );								//default constructor
	Request( vlocation *_locationVector, int inSock, char *buff );					//constructor
	Request( Request const & src );  				//copy
	~Request( void );								//destructor
	Request & operator=( Request const & rhs );		//overload operator =

	void	parseRequest( std::string req );
	void	updateTarget(std::string target);

	void	parseUrl();
	void	selectLocation();
	void	createPath();

	bool	isValidHeader( std::string header );
	bool	isRequestMethod( std::string key );
	void	printRequest();

	int				getInSock();
	std::string		getMethod();
	std::string		getTarget();
	std::string		getUrlTargetPath();
	std::string		getAbsoluteTargetPath();
	std::string		getQueryString();
	std::map<std::string, std::string> getSelectedLocation();
};

std::ostream &	operator<<(std::ostream & o, Request & rhs);

#endif
