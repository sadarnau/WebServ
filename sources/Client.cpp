#include "Client.hpp"

Client::Client( void )
{
	return ;
}

Client::Client( long socket, int servNb ) : _socket(socket), _servNb(servNb)
{
	this->_finishRead = false;
	this->_finishWrite = false;
	this->_buff.clear();

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
	{
		Logger::Write(Logger::INFO, RED, "server[" + Utils::intToStr(this->_servNb) + "] : read have gros problem");	
		return 0;
	}
	else if (ret == 0)
	{
		Logger::Write(Logger::INFO, RED, "server[" + Utils::intToStr(this->_servNb) + "] : client have closed his connection...");
		return (0);
	}
	else
		this->_buff += std::string(chunk_data);

	this->_finishRead = checkReadState();

	return (1);
}

bool	Client::checkReadState( void )
{
	unsigned long	i = this->_buff.find("\r\n\r\n");

	if (i != std::string::npos)
	{
		if (this->_buff.find("Transfer-Encoding: chunked") != std::string::npos)
		{
			if (Utils::checkLastChar(this->_buff, "0\r\n\r\n") == 0)
				return (true);
			else
				return (false);
		}
		else
			return (true);
	}

	return (false);
}

void	Client::deleteBuff( void )
{
	this->_buff.clear();	
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

void		Client::setFinishRead( bool finishRead )
{
	this->_finishRead = finishRead;
	return ;
}

std::ostream &	operator<<(std::ostream & o, Client & rhs)
{
	o << "fd = " << rhs.getSocket() << "\n";
	o << "serveur nb = " << rhs.getServerNb() << "\n";
	return (o);
}
