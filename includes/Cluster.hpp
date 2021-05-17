#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include "Client.hpp"
# include "Config.hpp"
# include "Webserv.hpp"
# include <map>
# include <sys/select.h>

class Cluster
{
private:

	long					_maxFd;
	fd_set					_master_fd;
	std::vector<Webserv>	_serverList;
	std::vector<int>		_fdList;
	std::vector<Client>		_clients;	//test
	std::vector<Client>		_readyClients;	//test
	Config					_config;
	int						_nbServ;

public:

	Cluster( void );								//default constructor
	// Cluster( std::string name );					//constructor
	Cluster( Cluster const & src);  				//copy
	~Cluster( void );								//destructor
	Cluster & operator=( Cluster const & rhs );		//overload operator =

	int									initialization( std::string fileName );
	int									lanchServices( void );

	void								addSocketToMaster( long socket );
	void								deleteInReadyClients( long socket );
	void								deleteInClients( long socket );
	void								setWritingSet( fd_set *writefds );

	std::map<std::string, std::string>	getMap( void );
	fd_set								getMasterSet( void );
	std::vector<Webserv>				getServerList( void );
	int									getMaxFd( void );
	std::vector<int>					getFdList( void );

	void								logCluster();
};

#endif
