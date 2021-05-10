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
	this->_maxFd = 0;	// not ouf du tout

	this->_config.parseFile(fileName);

	this->_serverList = this->_config.getServerVector();
	this->_nbServ = this->_serverList.size();

	FD_ZERO(&this->_master_fd);				// create a master file descriptor set and initialize it to zero

	for (int i = 0; i < this->_nbServ; i++)
	{
		Logger::Write(Logger::INFO, GRN, "server[" + std::to_string(i) + "] : creation");
		if (this->_serverList[i].initialization(i))
			return (1);
		FD_SET(this->_serverList[i].getFd(), &this->_master_fd);	// adding our first fd socket, the server one.
		if(this->_serverList[i].getFd() > this->_maxFd)				// ternaire ??
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
		// of living fds  (( man 2 select ))
		copyMasterSet = this->_master_fd;
		setWritingSet(&writingSet);

		int	ret = select(this->_maxFd + 1, &copyMasterSet, &writingSet, 0, 0);	// to do : check if there is an error in fd (errorfds)

		if (ret < 0)
		{
			Logger::Write(Logger::ERROR, RED, "error : select");
			throw (std::exception()); // to do : exception
			return (1); //test ???
		}

		// We go throught every server fds to see if we have a new connection
		for(int i = 0; i < this->_nbServ; i++)
		{
			if (FD_ISSET(this->_serverList[i].getFd(), &copyMasterSet)) // if serv fd changed -> new connection
			{
				Logger::Write(Logger::INFO, GRN, "server[" + std::to_string(i) + "] : new connection");
				int sock = this->_serverList[i].acceptConexion();
				addSocketToMaster(sock);
				break ;			// no need to check any more serv
			}
		}

		for(int i = 0; i < this->_nbServ; i++)
		{
			std::vector<int> list = this->_serverList[i].getFdList();
			for (std::vector<int>::iterator it = list.begin() ; it != list.end() ; it++)
			{
				if (FD_ISSET(*it, &copyMasterSet))
				{
					this->_serverList[i].handleRequest( *it );
					if (FD_ISSET(*it, &writingSet))
					{
						this->_serverList[i].sendResponse( *it );
					}
					else
					{
						Logger::Write(Logger::ERROR, RED, "server[" + std::to_string(i) + "] : error with fd[" + std::to_string(*it) + "]");
						close(*it);
						list.erase(it);
						this->_fdList.erase(it);
						return (1);
					}
				}
			}
		}

		// // We go throught every open fds to see if we have something new to read
		// for(int i = 0; i < this->_nbServ; i++)
		// {
		// 	std::vector<int> list = this->_serverList[i].getFdList();
		// 	for (std::vector<int>::iterator it = list.begin() ; it != list.end() ; it++)
		// 	{
		// 		if (FD_ISSET(*it, &copyMasterSet))
		// 		{
		// 			if (!findInVector(*it, this->_fdReady))
		// 				this->_serverList[i].handleRequest( *it );		//if so we send a response without checking if we can write...
		// 			else
		// 				Logger::Write(Logger::ERROR, RED, "Cannot write in this fd...");
		// 		}
		// 	}
		// }


		// std::vector<int> list = this->_fdList;
		// for (std::vector<int>::iterator it = list.begin() ; it != list.end() ; it++)
		// {
		// 	if (FD_ISSET(*it, &copyMasterSet))
		// 		this->_fdReady.push_back(*it);
		// }
	}

	return (0);
}

void								Cluster::addSocketToMaster( int socket )
{
	this->_fdList.push_back(socket);

	FD_SET(socket, &this->_master_fd);	// add the new fd in the master fd set

	if (socket > this->_maxFd)			// check until where we have to select
		this->_maxFd = socket;

	return ;
}

int								Cluster::findInVector( int socket, std::vector<int> _fdReady )
{
	std::vector<int> list = _fdReady;
	for (std::vector<int>::iterator it = list.begin() ; it != list.end() ; it++)
	{
		if (socket == *it)
			return (0);
	}

	return (1);
}

void							Cluster::setWritingSet( fd_set *writefds )
{
	FD_ZERO(writefds);
	std::vector<int> list = this->_fdList;
	for (std::vector<int>::iterator it = list.begin() ; it != list.end() ; it++)
		FD_SET(*it, writefds);
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

void							Cluster::logCluster(void)
{
	for (std::vector<Webserv>::iterator it = this->_serverList.begin(); it != this->_serverList.end(); ++it)
		it->logWebserv();
}