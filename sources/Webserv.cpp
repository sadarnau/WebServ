#include "Webserv.hpp"

Webserv::Webserv( void )
{
	return ;
}

Webserv::Webserv( std::string listen, std::vector<Location > locationVector, std::vector<Location> locationExtVector ) : _listen(listen), _locationVector(locationVector),
																															_locationExtVector(locationExtVector)
{
	return ;
}

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
	this->_fdList = rhs._fdList;
	this->_IPaddr = rhs._IPaddr;
	this->_port = rhs._port;
	this->fd = rhs.fd;
	this->address = rhs.address;
	this->_listen = rhs._listen;
	this->_locationVector = rhs._locationVector;
	this->_locationExtVector = rhs._locationExtVector;
	this->_serverNb = rhs._serverNb;

	return ( *this );
}

int		Webserv::initialization( int i )
{
	this->_serverNb = i;

	if ((this->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		Logger::Write(Logger::ERROR, RED, "Error : couldn't assign the socket...");
		return (1);
	}

	Logger::Write(Logger::INFO, GRN, "Socket created");
	
	this->fillAddress();
	
	// Fix binding error, it was due to TIME_WAIT who deosnt allow new connection to same socket before a certain time
	int reusePort = 1; // enum ?
	if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort)) < 0)
		return (1);

	if ((bind(this->fd, (struct sockaddr *)&this->address, sizeof(this->address))) < 0)
	{
		Logger::Write(Logger::ERROR, RED, "Error : couldn't bind the socket...");
		close(this->fd);
		return (1);
	}

	Logger::Write(Logger::INFO, GRN, "Socket : binded on " + this->_IPaddr + ":" + this->_port);

	if ((listen(this->fd, 10000)) < 0) 			// 10000 = number of max connections
	{
		Logger::Write(Logger::ERROR, RED, "Error : couldn't listen the socket");
		return (1);
	}

	Logger::Write(Logger::INFO, GRN, "Socket is listening...");

	return (0);
}

void	Webserv::fillAddress( void )
{
	this->_port = this->_listen.substr(this->_listen.find(":") + 1 , this->_listen.size());
	this->_IPaddr = this->_listen.substr(0, this->_listen.find(":"));

	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = inet_addr(this->_IPaddr.c_str());	//htonl ??
	this->address.sin_port = htons(std::stoi(this->_port));

	memset(this->address.sin_zero, 0, sizeof(this->address.sin_zero));

	return ;
}

int		Webserv::acceptConexion( void )
{
	struct sockaddr_in		address;
	unsigned int addrlen = sizeof(address);
	int	socket = accept(this->fd, (struct sockaddr *)&address, (socklen_t*)&addrlen); // to protect
	
	if (socket < 0)
	{
		Logger::Write(Logger::ERROR, RED, "Error : could not accept the new connection");
		return (-1);
	}
	
	fcntl(socket, F_SETFL, O_NONBLOCK);

	return (socket);
}

// int		Webserv::handleRequest( int socket )
// {
// 	int				BUFF_SIZE = 10000;
// 	int				ret;
// 	struct timeval	now , beginning;
// 	char			chunk_data[BUFF_SIZE];
// 	double			timediff;
	 
// 	memset(chunk_data , 0, BUFF_SIZE);
// 	ret = recv(socket, chunk_data, BUFF_SIZE - 1, 0);
	
// 	if (ret == 0)
// 	{
// 		Logger::Write(Logger::INFO, RED, "server[" + std::to_string(this->_serverNb) + "] : client have closed his connection...");
// 		return (0);
// 	}
// 	else
// 		this->_buff.append(chunk_data);

// 	return (1);
// }

void	Webserv::sendResponse( int socket, Client client )
{
	// char		 		myIP[16];
	// struct sockaddr_in	my_addr;

	// bzero(&my_addr, sizeof(my_addr));
	// socklen_t len = sizeof(my_addr);
	// getsockname(socket, (struct sockaddr *) &my_addr, &len);
	// inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
	// printf("Local ip address: %s\n", myIP);

	Request		request(&this->_locationVector, &this->_locationExtVector, socket, client.getBuffer());
	request.logRequest(this->_serverNb);

	Response	response(&request, socket);
	response.logResponse(this->_serverNb);

	return ;
}

// void	Webserv::deleteSocket( int socket )
// {
// 	for (std::vector<int>::iterator it = this->_fdList.begin() ; it != this->_fdList.end() ; it++)
// 	{
// 		if(*it == socket)
// 		{
// 			this->_fdList.erase(it);
// 			break ;
// 		}
// 	}

// 	return ;
// }

int		Webserv::getFd( void )
{
	return (this->fd);
}

int		Webserv::getMaxFd( void )
{
	return (this->_maxFd);
}

fd_set					Webserv::getMasterSet( void )
{
	return (this->_master_fd);
}

struct sockaddr_in		&Webserv::getAddr( void )
{
	return (this->address);
}

std::vector<int>		Webserv::getFdList( void )
{
	return (this->_fdList);
}

std::string				Webserv::getIpAddress( void )
{
	return (this->_IPaddr);
}

std::string				Webserv::getPort( void )
{
	return (this->_port);
}

std::vector<Location>	Webserv::getLocationVector( void )
{
	return (this->_locationVector);
}

std::vector<Location>	Webserv::getLocationExtVector( void )
{
	return (this->_locationExtVector);
}

int						Webserv::getServerNb( void )
{
	return (this->_serverNb);
}

void					Webserv::logWebserv()
{
	std::ostringstream oss;

	oss << "server[" << this->getServerNb() << "] : " ;
	oss << "[fd: " << this->getFd() << "] ";
	oss << "[IP address: " << this->getIpAddress() << "] ";
	oss << "[port: " << this->getPort() << "] ";

	Logger::Write(Logger::DEBUG, WHT, oss.str());

	for (std::vector<Location>::iterator it2 = this->_locationVector.begin(); it2 != this->_locationVector.end(); ++it2)
		it2->logLocation();
	for (std::vector<Location>::iterator it2 = this->_locationExtVector.begin(); it2 != this->_locationExtVector.end(); ++it2)
		it2->logLocation();

	return ;
}