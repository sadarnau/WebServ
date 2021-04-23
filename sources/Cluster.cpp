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

	this->_config.parseFile(fileName);

	printMap(this->getMap());
	
	FD_ZERO(&this->_master_fd);				//create a master file descriptor set and initialize it to zero

	for (int i = 0; i < nbServ; i++)
	{
		// Logger::Write(Logger::INFO, std::string(GRN), "Creating Server number " + std::to_string(i) + " !\n", true);
		Webserv webserv;
		webserv.initialization(this->_config);		//to do : return 1 or 0;
		FD_SET(webserv.getFd(), &this->_master_fd);	//adding our first fd socket, the server one.
		if(webserv.getFd() > this->_maxFd)
			this->_maxFd = webserv.getFd();
		this->_serverList.push_back(webserv);
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
	return (this->_config.getMap());
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