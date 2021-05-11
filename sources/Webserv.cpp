#include "Webserv.hpp"

Webserv::Webserv( void )
{
	return ;
}

Webserv::Webserv( std::string listen, std::vector<Location > locationVector ) : _listen(listen), _locationVector(locationVector)				//constructor
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
	this->_serverNb = rhs._serverNb;

	return ( *this );
}

int		Webserv::initialization( int i ) //to do : return 1 in case of error else return 0
{
	this->_serverNb = i;

	if ((this->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		Logger::Write(Logger::ERROR, RED, "error : assigning the socket");
		return (1);
	}

	Logger::Write(Logger::INFO, GRN, "socket : created");
	
	this->fillAddress();
	
	// Fix binding error, it was due to TIME_WAIT who deosnt allow new connection to same socket before a certain time
	int reusePort = 1; // enum ?
	setsockopt(this->fd, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort));	// to protect !

	if ((bind(this->fd, (struct sockaddr *)&this->address, sizeof(this->address))) < 0)
	{
		Logger::Write(Logger::ERROR, RED, "error : bindind the socket...");
		close(this->fd);
		return (1);
	}

	Logger::Write(Logger::INFO, GRN, "socket : binded on " + this->_IPaddr + ":" + this->_port);

	if ((listen(this->fd, 10000)) < 0) 			// 10000 = number of max connections
	{
		Logger::Write(Logger::ERROR, RED, "error : listening socket");
		return (1);
	}

	Logger::Write(Logger::INFO, GRN, "socket : listening...");

	return (0);
}

void	Webserv::fillAddress( void )
{
	this->_port = this->_listen.substr(this->_listen.find(":") + 1 , this->_listen.size());
	this->_IPaddr = this->_listen.substr(0, this->_listen.find(":"));

	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = inet_addr(this->_IPaddr.c_str());	//htonl ??
	this->address.sin_port = htons(std::stoi(this->_port));

	memset(this->address.sin_zero, 0, sizeof(this->address.sin_zero));	// to protect

	return ;
}

int		Webserv::acceptConexion( void )
{
	unsigned int addrlen = sizeof(this->address);
	int	socket = accept(this->fd, (struct sockaddr *)&this->address, (socklen_t*)&addrlen); // to protect
	
	if (socket < 0)
	{
		Logger::Write(Logger::ERROR, RED, "error : accepting a new connection");
		exit(1); // not ouf du tout...
	}
	
	fcntl(socket, F_SETFL, O_NONBLOCK);

	this->_fdList.push_back(socket);

	return (socket);
}

int	Webserv::handleRequest( int socket )
{
	int ret;
	struct timeval now , beginning;
	char chunk_data[128];
	double timediff;

	gettimeofday(&beginning , NULL);
	this->_buff.clear();
	 
	while(1)
	{
		gettimeofday(&now , NULL);
		timediff = (now.tv_sec - beginning.tv_sec) + 1e-6 * (now.tv_usec - beginning.tv_usec);
	 
		if(timediff > 0.2 ) 								// (0.2 is timeout)
			break;

		memset(chunk_data , 0, 128); 					 	// clear the variable (to do : protect)
		if((ret = recv(socket, chunk_data, 127, 0) ) < 0)	// to do : if = 0, client closed fd
			usleep(100000); 								// if nothing is received we wait 0.1 second before trying again
		else if (ret == 0)
		{
			Logger::Write(Logger::ERROR, RED, "Error : Client have closed his connection...");
			return (0);
		}
		else
		{
			this->_buff.append(chunk_data);
 			gettimeofday(&beginning , NULL);
		}
	}

	return 1;
}

void	Webserv::sendResponse( int socket )
{
	Request		request(&this->_locationVector, socket, this->_buff);
	request.logRequest(this->_serverNb);

	Response	response(&request, socket);
	response.logResponse(this->_serverNb);

	return ;
}

void	Webserv::deleteSocket( int socket )
{
	for (std::vector<int>::iterator it = this->_fdList.begin() ; it != this->_fdList.end() ; it++)
	{
		if(*it == socket)
		{
			this->_fdList.erase(it);
			break ;
		}
	}
}

int		Webserv::getFd( void )
{
	return (this->fd);
}

int		Webserv::getMaxFd( void )
{
	return (this->_maxFd);
}

fd_set								Webserv::getMasterSet( void )
{
	return (this->_master_fd);
}

struct sockaddr_in					&Webserv::getAddr( void )
{
	return (this->address);
}

std::vector<int>					Webserv::getFdList( void )
{
	return (this->_fdList);
}

std::string							Webserv::getIpAddress( void )
{
	return (this->_IPaddr);
}

std::string							Webserv::getPort( void )
{
	return (this->_port);
}

std::vector<Location>	Webserv::getLocationVector( void )
{
	return (this->_locationVector);
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
}