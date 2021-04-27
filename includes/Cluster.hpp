#ifndef CLUSTER_HPP
# define CLUSTER_HPP

# include "Config.hpp"
# include "Webserv.hpp"
# include <map>
# include <sys/select.h>

class Cluster
{
private:
	
	int						_maxFd;
	fd_set					_master_fd;
	std::vector<Webserv>	_serverList;
	std::vector<int>		_fdList;
	Config					_config;
	int						_nbServ; // ONLY A TEST

public:

	Cluster( void );								//default constructor
	// Cluster( std::string name );					//constructor
	Cluster( Cluster const & src);  				//copy
	~Cluster( void );								//destructor
	Cluster & operator=( Cluster const & rhs );		//overload operator =

	int									initialization( std::string fileName );
	int									lanchServices( void );
	void								addSocketToMaster( int socket );
	void								requestPrintServ( void );

	std::map<std::string, std::string>	getMap( void );
	fd_set								getMasterSet( void );
	std::vector<Webserv>				getServerList( void );
	int									getMaxFd( void );
	std::vector<int>					getFdList( void );
};

std::ostream &	operator<<(std::ostream & o, Cluster & rhs);

#endif
