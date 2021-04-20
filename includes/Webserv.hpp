#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "Request.hpp"
# include "Logger.hpp"
# include "Config.hpp"
# include "Response.hpp"
# include <unistd.h>		//read function
# include <arpa/inet.h>		//inet_addr function
# include <string>
# include <iostream>
# include <cwchar>
# include <cstring>
# include <netinet/in.h>	//sockaddr struct
# include <sys/select.h>	//FD_ZERO

class Webserv
{
private:

	int					fd;
	std::vector<int>	_fdList;
	struct sockaddr_in	address;
	Request				inRequest;
	Config				config;
	fd_set				_master_fd;
	int					_maxFd;
public:

	Webserv( void );						 		//default constructor
	// Webserv( std::string name );					//constructor
	Webserv( Webserv const & src);  				//copy
	~Webserv( void );								//destructor
	Webserv & operator=( Webserv const & rhs );		//overload operator =

	void								initialization( std::string fileName );
	void								fillAddress( void );
	void								acceptConexion( void );
	void								handleRequest( int socket );
	int									getInSocket( void );
	int									getFd( void );
	int									getMaxFd( void );
	fd_set								getMasterSet( void );
	std::map<std::string, std::string>	getMap( void );
	struct sockaddr_in			&		getAddr( void );
	Config						&		getConfig( void );
	std::vector<int>					getFdList( void );

};

#endif
