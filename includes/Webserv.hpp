#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "Request.hpp"
# include "Logger.hpp"
# include "Config.hpp"
# include <unistd.h>		//read function
# include <arpa/inet.h>		//inet_addr function
# include <string>
# include <iostream>
# include <cwchar>
# include <cstring>
# include <netinet/in.h>     //sockaddr struct

class Webserv
{
private:

	int					fd;
	struct sockaddr_in	address;
	Request				inRequest;
	Config				config;

public:

	Webserv( void );						 		//default constructor
	// Webserv( std::string name );					//constructor
	Webserv( Webserv const & src);  				//copy
	~Webserv( void );								//destructor
	Webserv & operator=( Webserv const & rhs );		//overload operator =

	void	initialization( std::string fileName );
	void	fillAddress( void );
	void	handleRequest( void );
	int		getInSocket( void );
	int		getFd( void );
	std::map<std::string, std::string>	getMap( void );
};

#endif
