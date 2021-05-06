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

	if ((listen(this->fd, 5)) < 0) 			// 5 = number of max connections (why 5 ??)
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

	this->_fdList.push_back(socket);

	return (socket);
}

void	Webserv::handleRequest( int socket )
{
	// consider socket like a stream, the request can be send in multiple packets (for big request)
	// so this version is KO

	std::cout << this->getServerNb() << "\n\n";

	char buff[1024];						// 1024 ????
	int ret = read( socket , buff, 1024);	// to protect

	buff[ret] = 0;	// realy usefull ?
	this->_buff = buff;
}

void	Webserv::sendResponse( int socket )
{
	Request		request(&this->_locationVector, socket, this->_buff);
	Logger::Write(Logger::INFO, BLU, "server[" + std::to_string(this->_serverNb) + "] : request received [method: " + request.getMethod() + "] [location: " + request.getSelectedLocation().getPath() + "] [target: " + request.getTarget() + "]");
	Response	response(&request, socket);
	Logger::Write(Logger::INFO, BLU, "server[" + std::to_string(this->_serverNb) + "] : response sent [code: " + response.getResponseCodeStr() + "] [message: " + response.getResponseCodeMessage() + "] [content length: " + response.getContentLength()+ "]");
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


std::ostream &	operator<<(std::ostream & o, Webserv & rhs)
{
	o << "server[" << rhs.getServerNb() << "] : " ;
	o << "[fd: " << rhs.getFd() << "] ";
	o << "[IP address: " << rhs.getIpAddress() << "] ";
	o << "[port: " << rhs.getPort() << "] ";
	// o << "Config = " << rhs.getConfig() << "\n\n";
	return ( o );
}
