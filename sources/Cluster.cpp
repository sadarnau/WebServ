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

int		Cluster::initialization( std::string fileName )
{
	this->nbServ = 1;	// to test ONLY
	this->_maxFd = 0;	// not ouf du tout

	this->_config.parseFile(fileName); // renvoyer la list des serveurs !

	printMap(this->getMap());
	
	FD_ZERO(&this->_master_fd);				//create a master file descriptor set and initialize it to zero

	for (int i = 0; i < nbServ; i++)
	{
		// Logger::Write(Logger::INFO, std::string(GRN), "Creating Server number " + std::to_string(i) + " !\n", true);
		Webserv webserv;
		if (webserv.initialization(this->_config))
			return 1;
		FD_SET(webserv.getFd(), &this->_master_fd);	//adding our first fd socket, the server one.
		if(webserv.getFd() > this->_maxFd)
			this->_maxFd = webserv.getFd();
		this->_serverList.push_back(webserv);
	}

	return (0);
}

void								Cluster::lanchServices( void )
{
	fd_set	copyMasterSet;

	int serverFd = this->_serverList[0].getFd();

	// test :
	Webserv webserv = this->_serverList[0];

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
			exit (1); //test
		}

		if (FD_ISSET(serverFd, &copyMasterSet)) // if serv fd changed -> new connection
		{
			Logger::Write(Logger::INFO, std::string(GRN), "New connection !\n\n", true);
			int sock = this->_serverList[0].acceptConexion();
			addSocketToMaster(sock);
		}

		// We go throught every open fds to see if we have something new to read
		std::vector<int> list = this->_fdList;
		for (std::vector<int>::iterator it = list.begin() ; it != list.end() ; it++)
		{
			if (FD_ISSET(*it, &copyMasterSet))
				webserv.handleRequest( *it );		//if so we send a response without checking if we can write...
		}
	}

	return ;
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