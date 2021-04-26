#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "Request.hpp"
# include "Response.hpp"
# include "Logger.hpp"
# include "Config.hpp"
# include <unistd.h>		//read function
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

	int					fd;
	std::vector<int>	_fdList;
	struct sockaddr_in	address;
	Request				inRequest;
	std::map<std::string, std::string>	_configMap;
	std::vector<std::map<std::string, std::string> > _locationVector;
	fd_set				_master_fd;
	int					_maxFd;
	std::string			_port;
	std::string			_IPaddr;

public:

	Webserv( void );						 		//default constructor
	Webserv( std::map<std::string, std::string>	configMap, std::vector<std::map<std::string, std::string> > locationVector );					//constructor
	Webserv( Webserv const & src);  				//copy
	~Webserv( void );								//destructor
	Webserv & operator=( Webserv const & rhs );		//overload operator =

	int									initialization( void );
	void								fillAddress( void );
	int									acceptConexion( void );
	void								handleRequest( int socket );
	int									getInSocket( void );
	int									getFd( void );
	int									getMaxFd( void );
	fd_set								getMasterSet( void );
	std::map<std::string, std::string>	getConfigMap( void );
	struct sockaddr_in					&getAddr( void );
	std::vector<int>					getFdList2( void );
	std::string							getIpAddress( void );
	std::string							getPort( void );
};

std::ostream &	operator<<(std::ostream & o, Webserv & rhs);

#endif
