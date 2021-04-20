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

void	Webserv::initialization( std::string fileName ) //to do : return 1 in case of error else return 0
{
	this->config.parseFile(fileName);

	printMap(this->getMap());
	if ((this->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		Logger::Write(Logger::ERROR, std::string(RED), "Error assigning the socket...\n", true);
		exit(1);
	}

	Logger::Write(Logger::INFO, std::string(GRN), "The socket has been created !\n", true);
	
	this->fillAddress();

	if ((bind(this->fd, (struct sockaddr *)&this->address, sizeof(this->address))) < 0)
	{
		Logger::Write(Logger::ERROR, std::string(RED), "Error bindind the socket...\n", true);
		exit(1);
	}

	Logger::Write(Logger::INFO, std::string(GRN), "The socket has been binded !\n", true);

	if ((listen(this->fd, 5)) < 0) // 5 = number of max connections
	{
		Logger::Write(Logger::ERROR, std::string(RED), "Error listening the socket...\n", true);
		exit(1);
	}

	Logger::Write(Logger::INFO, std::string(GRN), "Listening the socket !\n", true); //better to IP::PORT

	FD_ZERO(&this->_master_fd);	//create a master file descriptor set and initialize it to zero

	FD_SET(this->fd, &this->_master_fd);	//adding our first fd socket, the listening one.
	
	this->_maxFd = this->fd;

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

	char buff[1024];				// GNL maybe better ?
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

int		Webserv::getMaxFd( void )
{
	return (this->_maxFd);
}

std::map<std::string, std::string>	Webserv::getMap( void )
{
	return (this->config.getMap());
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
