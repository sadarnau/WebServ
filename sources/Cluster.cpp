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

int								Cluster::initialization( std::string fileName )
{
	this->_maxFd = 0;	// not ouf du tout

	this->_config.parseFile(fileName);
	this->_serverList = this->_config.getServerVector();
	// printMap(this->getMap());
	
	this->_nbServ = this->_serverList.size();

	FD_ZERO(&this->_master_fd);				// create a master file descriptor set and initialize it to zero

	for (int i = 0; i < this->_nbServ; i++)
	{
		Logger::Write(Logger::INFO, std::string(GRN), "Creating Server number " + std::to_string(i) + " !\n", true);
		if (this->_serverList[i].initialization(i))
			return 1;
		FD_SET(this->_serverList[i].getFd(), &this->_master_fd);	// adding our first fd socket, the server one.
		if(this->_serverList[i].getFd() > this->_maxFd)				// ternaire ??
			this->_maxFd = this->_serverList[i].getFd();
	}

	printAllServers(this->_serverList);

	return (0);
}

int								Cluster::lanchServices( void )
{
	fd_set	copyMasterSet;

	// test :
	// Webserv webserv = this->_serverList[0];

	// std::cout << *this;
	// std::cout << webserv;

	while(1)
	{
		// We have to make a copy of the master fd set because select() will change bits
		// of living fds  (( man 2 select ))
		copyMasterSet = this->_master_fd;

		Logger::Write(Logger::INFO, std::string(GRN), "I'm waiting for a request...\n\n", true);

		int	nbSocket = select(this->_maxFd + 1, &copyMasterSet, 0, 0, 0); //to do : check if we can write in fd (writefds)

		if (nbSocket < 0)
		{
			Logger::Write(Logger::ERROR, std::string(RED), "Select has messed up everything...\n", true);
			throw (std::exception()); // to do : exception
			return (1); //test ???
		}

		for(int i = 0; i < this->_nbServ; i++)
		{
			if (FD_ISSET(this->_serverList[i].getFd(), &copyMasterSet)) // if serv fd changed -> new connection
			{
				Logger::Write(Logger::INFO, std::string(GRN), "New connection on serv " + std::to_string(i) + " !\n", true);
				int sock = this->_serverList[i].acceptConexion();
				addSocketToMaster(sock);
				break ;			// no need to check any more serv
			}
		}

		// We go throught every open fds to see if we have something new to read
		for(int i = 0; i < this->_nbServ; i++)
		{
			std::vector<int> list = this->_serverList[i].getFdList();
			for (std::vector<int>::iterator it = list.begin() ; it != list.end() ; it++)
			{
				if (FD_ISSET(*it, &copyMasterSet))
					this->_serverList[i].handleRequest( *it );		//if so we send a response without checking if we can write...
			}
		}
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

std::ostream &	operator<<(std::ostream & o, Cluster & rhs)
{
	o << "In the cluster we have :\n";
	o << "_maxFd = " << rhs.getMaxFd() << '\n';
	o << "nb of server = " << rhs.getServerList().size() << "\n\n";
	// o << "_config = " << rhs.getConfig() << '\n';

	return ( o );
}