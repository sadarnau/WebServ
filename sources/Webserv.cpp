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

int		Webserv::initialization( std::string fileName ) //to do : return 1 in case of error else return 0
{
	this->config.parseFile(fileName);

	printMap(this->getMap());
	
	if ((this->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		Logger::Write(Logger::ERROR, std::string(RED), "Error assigning the socket...\n", true);
		return (1);
	}

	Logger::Write(Logger::INFO, std::string(GRN), "The socket has been created !\n", true);
	
	this->fillAddress();
	
	// Fix binding error, it was due to TIME_WAIT who deosnt allow new connection to same socket before a certain time
	int reusePort = 1;
	setsockopt(this->fd, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort));

	if ((bind(this->fd, (struct sockaddr *)&this->address, sizeof(this->address))) < 0)
	{
		Logger::Write(Logger::ERROR, std::string(RED), "Error bindind the socket...\n", true);
		close(this->fd);
		return (1);
	}

	Logger::Write(Logger::INFO, std::string(GRN), "The socket has been binded on : " + this->_IPaddr + ':' + this->_port + " !\n", true);

	if ((listen(this->fd, 5)) < 0) 			// 5 = number of max connections
	{
		Logger::Write(Logger::ERROR, std::string(RED), "Error listening the socket...\n", true);
		return (1);
	}

	Logger::Write(Logger::INFO, std::string(GRN), "Listening the socket !\n", true);

	FD_ZERO(&this->_master_fd);				//create a master file descriptor set and initialize it to zero

	FD_SET(this->fd, &this->_master_fd);	//adding our first fd socket, the server one.
	
	this->_maxFd = this->fd;

	return (0);
}

void	Webserv::fillAddress( void )
{
	std::map<std::string, std::string> configMap = this->config.getConfigMap();
	
	this->_port = configMap["listen"].substr(configMap["listen"].find(":") + 1 , configMap["listen"].size());
	this->_IPaddr = configMap["listen"].substr(0, configMap["listen"].find(":"));

	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = inet_addr(this->_IPaddr.c_str());	//htonl ??
	this->address.sin_port = htons(std::stoi(this->_port));

	memset(this->address.sin_zero, 0, sizeof(this->address.sin_zero));	// to pprotect

	return ;
}

void	Webserv::acceptConexion( void )
{
	unsigned int addrlen = sizeof(address);
	int	socket = accept(this->fd, (struct sockaddr *)&this->address, (socklen_t*)&addrlen);
	
	if (socket < 0)
	{
		Logger::Write(Logger::ERROR, std::string(RED), "Error accepting a new connection\n", true);
		exit(1);
	}

	this->_fdList.push_back(socket);

	FD_SET(socket, &this->_master_fd);	// add the new fd in the master fd set

	if (socket > this->_maxFd)			// check until where we have to select
		this->_maxFd = socket;
}

void	Webserv::handleRequest( int socket )
{
	// consider socket like a stream, the request can be send in multiple packets (for big request)
	// so this version is KO
	
	char buff[1024];						// GNL maybe better ?
	int ret = read( socket , buff, 1024);	// to protect
	
	buff[ret] = 0;

	Request		request(&this->config, socket, buff);

	Response	response(&this->config, &request, socket);	
	response.buildResponse();
	response.send();

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

int		Webserv::getMaxFd( void )
{
	return (this->_maxFd);
}

std::map<std::string, std::string>	Webserv::getMap( void )
{
	return (this->config.getConfigMap());
}

fd_set								Webserv::getMasterSet( void )
{
	return (this->_master_fd);
}

struct sockaddr_in					&Webserv::getAddr( void )
{
	return (this->address);
}

Config								&Webserv::getConfig( void )
{
	return (this->config);
}

std::vector<int>					Webserv::getFdList( void )
{
	return (this->_fdList);
}
