#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "Request.hpp"
# include "Response.hpp"
# include "Logger.hpp"
# include "Config.hpp"
# include "Location.hpp"
# include <unistd.h>		//read function
# include <fcntl.h>
# include <arpa/inet.h>		//inet_addr function
# include <string>
# include <iostream>
# include <cwchar>
# include <cstring>
# include <netinet/in.h>	//sockaddr struct
# include <sys/select.h>	//FD_ZERO

class Request ;

class Webserv
{
private:

	int						fd;
	std::string				_buff; // test
	std::vector<int>		_fdList;
	struct sockaddr_in		address;
	std::string				_listen;
	std::vector<Location>	_locationVector;
	fd_set					_master_fd;
	int						_maxFd;
	std::string				_port;
	std::string				_IPaddr;
	int						_serverNb;

public:

	Webserv( void );						 		//default constructor
	Webserv( std::string listen, std::vector<Location> locationVector );					//constructor
	Webserv( Webserv const & src);  				//copy
	~Webserv( void );								//destructor
	Webserv & operator=( Webserv const & rhs );		//overload operator =

	int									initialization( int i );
	void								fillAddress( void );
	int									acceptConexion( void );
	void								handleRequest( int socket );
	void								sendResponse( int socket );

	int									getFd( void );
	int									getMaxFd( void );
	fd_set								getMasterSet( void );
	struct sockaddr_in					&getAddr( void );
	std::vector<int>					getFdList( void );
	std::string							getIpAddress( void );
	std::string							getPort( void );
	std::string							getListen( void );
	std::vector<Location>				getLocationVector( void );
	int									getServerNb( void );
};

std::ostream &	operator<<(std::ostream & o, Webserv & rhs);

#endif
