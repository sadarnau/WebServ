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

class Request
{
private:

	std::vector<std::map<std::string, std::string> > *_locationVector;
	int			_inSocket;
	std::string	_buff;

	std::string _method;
	std::string _target;
	std::string _relativeTargetPath;
	std::string _absoluteTargetPath;
	std::map<std::string, std::string> _headers;
	std::vector<std::string> _skippedHeaders;

public:

	Request( void );								//default constructor
	Request( std::vector<std::map<std::string, std::string> > *_locationVector, int inSock, char *buff );					//constructor
	Request( Request const & src );  				//copy
	~Request( void );								//destructor
	Request & operator=( Request const & rhs );		//overload operator =

	void	parseRequest( std::string req );

	void	createPath();
	bool	isValidHeader( std::string header );
	bool	isRequestMethod( std::string key );
	void	printRequest();

	int				getInSock();
	std::string		getMethod();
	std::string		getTarget();
	std::string		getRelativeTargetPath();
	std::string		getAbsoluteTargetPath();
	void			updateTarget(std::string target);
};

std::ostream &	operator<<(std::ostream & o, Request & rhs);

#endif
