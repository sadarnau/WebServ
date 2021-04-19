#include "Webserv.hpp"

Webserv::Webserv( void )
{
	return ;
}

// Webserv::Webserv( std::string name )
// {
// 	return ;
// }

Webserv::Webserv( Webserv const & src )
{
	*this = src;
	return ;
}

Webserv::~Webserv( void )
{
	return ;
}

Webserv & Webserv::operator=( Webserv const & rhs)
{
    // this->??? = rhs.???;
	(void)rhs;
	return ( *this );
}

void	Webserv::initialization( std::string fileName )
{
	this->config.parseFile(fileName);

	this->fd = socket(AF_INET, SOCK_STREAM, 0); // to pprotect
	
	this->fillAddress();

	bind(this->fd, (struct sockaddr *)&this->address, sizeof(this->address));  // to protect

	listen(this->fd, 5); // 5 = number of max connections
	
	return ;
}

void	Webserv::fillAddress( void )
{
	std::map<std::string, std::string> configMap = this->config.getMap();
	
	std::string port = configMap["listen"].substr(configMap["listen"].find(":") + 1 , configMap["listen"].size());
	std::string	IPaddr = configMap["listen"].substr(0, configMap["listen"].find(":"));

	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = inet_addr(IPaddr.c_str()); //htonl ??
	this->address.sin_port = htons(std::stoi(port));

	memset(this->address.sin_zero, 0, sizeof(this->address.sin_zero));  // to pprotect

	return ;
}

void	Webserv::handleRequest( void )
{
	int in_sock;

	unsigned int addrlen = sizeof(address);
	in_sock = accept(this->fd, (struct sockaddr *)&this->address, (socklen_t*)&addrlen);
	
	char buff[1000];				// GNL maybe better ?
	int ret = read( in_sock , buff, 1000);	// to protect
	
	buff[ret] = 0;

	Request inRequest(in_sock, buff);
	this->inRequest = inRequest;
}

int		Webserv::getInSocket( void )
{
	return (this->inRequest.getInSock());
}

int		Webserv::getFd( void )
{
	return (this->fd);
}

std::map<std::string, std::string>	Webserv::getMap( void )
{
	return (this->config.getMap());
}
