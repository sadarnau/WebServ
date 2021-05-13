#include "Client.hpp"

Client::Client( void )
{
	return ;
}

Client::Client( int socket, int servNb ) : _socket(socket), _servNb(servNb)
{
	this->_finishRead = true;
	this->_finishWrite = true;

	return ;
}

Client::Client( Client const & src )
{
	*this = src;
	return ;
}

Client::~Client( void )
{
	return ;
}

Client & Client::operator=( Client const & rhs)
{
    this->_buff = rhs._buff;
    this->_finishRead = rhs._finishRead;
    this->_finishWrite = rhs._finishWrite;
    this->_servNb = rhs._servNb;
    this->_socket = rhs._socket;
	return ( *this );
}

int		Client::myRecv( void )
{
	int				BUFF_SIZE = 100000;
	int				ret;
	char			chunk_data[BUFF_SIZE];

	memset(chunk_data , 0, BUFF_SIZE);
	if ((ret = recv(this->_socket, chunk_data, BUFF_SIZE - 1, 0)) < 0)
		return 0;	// read error, close connection
	else if (ret == 0)
	{
		Logger::Write(Logger::INFO, RED, "server[" + std::to_string(this->_servNb) + "] : client have closed his connection...");
		return (0);
	}
	else
		this->_buff.append(chunk_data);

	this->_finishRead = checkReadState();

	return (1);
}

bool	Client::checkReadState( void )
{
	// check if the request is full
	// set the flag this->finishRead to true or false
	return (true);
}

int								checkEnd(const std::string& str, const std::string& end)
{
	size_t	i = str.size();
	size_t	j = end.size();

	while (j > 0)
	{
		i--;
		j--;
		if (i < 0 || str[i] != end[j])
			return (1);
	}
	return (0);
}

int		Client::getServerNb( void )
{
	return (this->_servNb);
}

int		Client::getSocket( void )
{
	return (this->_socket);
}

bool	Client::getFinishRead( void )
{
	return (this->_finishRead);
}

bool	Client::getFinishWrite( void )
{
	return (this->_finishWrite);
}

std::string	Client::getBuffer( void )
{
	return (this->_buff);
}

void		Client::setFinishWrite( bool finishWrite )
{
	this->_finishWrite = finishWrite;
	return ;
}

std::ostream &	operator<<(std::ostream & o, Client & rhs)
{
	o << "fd = " << rhs.getSocket() << "\n";
	o << "serveur nb = " << rhs.getServerNb() << "\n";
	return (o);
}
