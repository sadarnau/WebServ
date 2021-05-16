#include "Cluster.hpp"

Cluster::Cluster( void )
{
	return ;
}

// Cluster::Cluster( std::string name )
// {
// 	return ;
// }

Cluster::Cluster( Cluster const & src )
{
	*this = src;

	return ;
}

Cluster::~Cluster( void )
{
	return ;
}

Cluster & Cluster::operator=( Cluster const & rhs)
{
	(void)rhs;
    // this->??? = rhs.???;

	return ( *this );
}

int								Cluster::initialization( std::string fileName)
{
	this->_maxFd = 0;
	this->_config.parseFile(fileName);

	this->_serverList = this->_config.getServerVector();
	this->_nbServ = this->_serverList.size();

	FD_ZERO(&this->_master_fd);										// create a master file descriptor set and initialize it to zero

	for (int i = 0; i < this->_nbServ; i++)
	{
		Logger::Write(Logger::INFO, GRN, "server[" + Utils::intToStr(i) + "] : creation");
		if (this->_serverList[i].initialization(i))
			return (1);
		FD_SET(this->_serverList[i].getFd(), &this->_master_fd);	// adding our first fd socket, the server one.
		if(this->_serverList[i].getFd() > this->_maxFd)				// ternaire ?? a faire
			this->_maxFd = this->_serverList[i].getFd();
	}

	this->logCluster();

	return (0);
}

int								Cluster::lanchServices( void )
{
	fd_set	copyMasterSet;
	fd_set	writingSet;

	while(1)
	{
		// We have to make a copy of the master fd set because select() will change bits
		// of moving fds  (( man 2 select ))
		copyMasterSet = this->_master_fd;
		setWritingSet(&writingSet);

		int	ret = select(this->_maxFd + 1, &copyMasterSet, &writingSet, 0, 0);

		if (ret < 0)
		{
			Logger::Write(Logger::ERROR, RED, "Error : select have deconné");
			throw (std::exception()); // to do : exception
			return (1); // test ???
		}
		
		for (std::vector<Client>::iterator it = this->_readyClients.begin(); it != this->_readyClients.end(); it++)
		{
			if (FD_ISSET(it->getSocket(), &writingSet))
			{
				this->_serverList[it->getServerNb()].sendResponse( it->getSocket(), *it );
				it->setFinishWrite(true);	// test
				if (it->getFinishWrite())
					this->_readyClients.erase(it);
			}
			else
			{
				Logger::Write(Logger::ERROR, RED, "server[" + std::to_string(it->getServerNb()) + "] : error with send, fd[" + std::to_string(it->getSocket()) + "]");
				if (close(it->getSocket()) < 0)
					return (1);
				FD_CLR(it->getSocket(), &this->_master_fd);
				FD_CLR(it->getSocket(), &writingSet);
				this->_readyClients.erase(it);
			}
			break;
		}

		for(int i = 0; i < this->_nbServ; i++)							// We go throught every server fds to see if we have a new connection
			if (FD_ISSET(this->_serverList[i].getFd(), &copyMasterSet))	// if serv fd changed -> new connection
			{
				int	sock;
				Logger::Write(Logger::INFO, GRN, "server[" + Utils::intToStr(i) + "] : new connection");
				if ((sock = this->_serverList[i].acceptConexion()) < 0)
					return (1); // test ???

				FD_SET(sock, &this->_master_fd);	// add the new fd in the master fd set
				if (sock > this->_maxFd)			// check until where we have to select
					this->_maxFd = sock;

				this->_clients.push_back(Client(sock, i));
				break ;								// no need to check any more serv
			}

		// std::vector<Client> list = this->_clients;
		for (std::vector<Client>::iterator it = this->_clients.begin() ; it != this->_clients.end() ; it++)
		{
			if (FD_ISSET(it->getSocket(), &copyMasterSet))
			{
				if ( !it->myRecv() )
				{
					if (close(it->getSocket()) < 0)
						return (1);
					FD_CLR(it->getSocket(), &this->_master_fd);
					this->_clients.erase(it);
				}
				else
					if (it->getFinishRead())
					{
						// Logger::Write(Logger::INFO, GRN, "Read is finished, preparing to send " + std::to_string(it->getBuffer().size()) + " bits :\n" + it->getBuffer());
						this->_readyClients.push_back(*it);
						it->setFinishRead(false);
						it->deleteBuff();
					}
			}
			break;
		}
	}

	return (0);
}

void								Cluster::deleteInReadyClients( int socket )
{
	for (std::vector<Client>::iterator it = this->_readyClients.begin() ; it != this->_readyClients.end() ; it++)
		if (socket == it->getSocket())
		{
			this->_readyClients.erase(it);
			break ;
		}
	
	return ;
}

void								Cluster::deleteInClients( int socket )
{
	for (std::vector<Client>::iterator it = this->_clients.begin() ; it != this->_clients.end() ; it++)
		if (socket == it->getSocket())
		{
			this->_clients.erase(it);
			break ;
		}
	
	return ;
}

void							Cluster::setWritingSet( fd_set *writefds )
{
	FD_ZERO(writefds);
	std::vector<Client> list = this->_readyClients;
	for (std::vector<Client>::iterator it = list.begin() ; it != list.end() ; it++)
		FD_SET(it->getSocket(), writefds);

	return ;
}

std::map<std::string, std::string>	Cluster::getMap( void )
{
	return (this->_config.getConfigMap());
}

fd_set								Cluster::getMasterSet( void )
{
	return (this->_master_fd);
}

std::vector<Webserv>				Cluster::getServerList( void )
{
	return (this->_serverList);
}

int									Cluster::getMaxFd( void )
{
	return (this->_maxFd);
}

std::vector<int>					Cluster::getFdList( void )
{
	return (this->_fdList);
}

void								Cluster::logCluster(void)
{
	for (std::vector<Webserv>::iterator it = this->_serverList.begin(); it != this->_serverList.end(); ++it)
		it->logWebserv();
}