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
	std::map<std::string, std::string> configMap = this->config.getConfigMap();
	
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
	int socket;

	unsigned int addrlen = sizeof(address);
	socket = accept(this->fd, (struct sockaddr *)&this->address, (socklen_t*)&addrlen);
	
	char buff[1000];				// GNL maybe better ?
	int ret = read( socket , buff, 1000);	// to protect
	
	buff[ret] = 0;

	Request request(socket, buff);

	Response	response(&this->config, &request, socket);	
	response.buildResponse();
	response.send();

	//printing the response on the terminal
	Logger::Write(Logger::DEBUG, std::string(BLU), "\n---------\nResponse:\n\n" + response.getResponse() + "\n-------\n\n", true);
	Logger::Write(Logger::INFO, std::string(GRN), "Message delivered...\n\n", true);
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
	return (this->config.getConfigMap());
}
