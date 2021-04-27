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
		Logger::Write(Logger::ERROR, std::string(RED), "Error assigning the socket...\n", true);
		return (1);
	}

	Logger::Write(Logger::INFO, std::string(GRN), "The socket has been created !\n", true);
	
	this->fillAddress();
	
	// Fix binding error, it was due to TIME_WAIT who deosnt allow new connection to same socket before a certain time
	int reusePort = 1; // enum ?
	setsockopt(this->fd, SOL_SOCKET, SO_REUSEPORT, &reusePort, sizeof(reusePort));	// to protect !

	if ((bind(this->fd, (struct sockaddr *)&this->address, sizeof(this->address))) < 0)
	{
		Logger::Write(Logger::ERROR, std::string(RED), "Error bindind the socket...\n", true);
		close(this->fd);
		return (1);
	}

	Logger::Write(Logger::INFO, std::string(GRN), "The socket has been binded on : " + this->_IPaddr + ':' + this->_port + " !\n", true);

	if ((listen(this->fd, 5)) < 0) 			// 5 = number of max connections (why 5 ??)
	{
		Logger::Write(Logger::ERROR, std::string(RED), "Error listening the socket...\n", true);
		return (1);
	}

	Logger::Write(Logger::INFO, std::string(GRN), "Listening the socket !\n", true);

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
		Logger::Write(Logger::ERROR, std::string(RED), "Error accepting a new connection\n", true);
		exit(1); // not ouf du tout...
	}

	this->_fdList.push_back(socket);

	return (socket);
}

void	Webserv::handleRequest( int socket )
{
	// consider socket like a stream, the request can be send in multiple packets (for big request)
	// so this version is KO

	char buff[1024];						// 1024 ????
	int ret = read( socket , buff, 1024);	// to protect
	
	buff[ret] = 0;	// realy usefull ?

	Request		request(&this->_locationVector, socket, buff);

	Response	response(&request, socket);

	response.buildResponse();
	response.send();

	Logger::Write(Logger::DEBUG, std::string(BLU), "\n---------\nRESPONSE HEADER :\n\n" + response.getHeader() + "\n-------\n\n", true);
	Logger::Write(Logger::MORE, std::string(BLU), "\n---------\nRESPONSE BODY :\n\n" + response.getBody() + "\n-------\n\n", true);
	Logger::Write(Logger::INFO, std::string(GRN), "Response delivered by server number " + std::to_string(this->_serverNb) + " !\n", true);
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


std::ostream &	operator<<(std::ostream & o, Webserv & rhs)
{
	o << "In this server we have :\n";
	o << "fd = " << rhs.getFd() << '\n';
	o << "IP address = " << rhs.getIpAddress() << "\n";
	o << "port = " << rhs.getPort() << "\n\n";
	// o << "Config = " << rhs.getConfig() << "\n\n";

	return ( o );
}
