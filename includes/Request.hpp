#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>
# include <map>
# include <vector>
# include <sstream>
# include <colors.hpp>
# include <iomanip>
# include "Logger.hpp"

class Request
{
private:

	int			_inSocket;
	std::string	_buff;

	std::string _method;
	std::string _relativeTargetPath;
	std::string _absoluteTargetPath;
	std::map<std::string, std::string> _headers;
	std::vector<std::string> _skippedHeaders;

public:

	Request( void );								//default constructor
	Request( int inSock, char *buff );					//constructor
	Request( Request const & src);  				//copy
	~Request( void );								//destructor
	Request & operator=( Request const & rhs );		//overload operator =

	bool	isValidHeader(std::string header);
	bool	isRequestMethod(std::string key);
	void	parseRequest(std::string req);
	void	printRequest( void );

	int		getInSock( void );
	std::string		getMethod();
	std::string		getTarget();
};

#endif
