#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <fstream>
# include <sstream>
# include <map>
# include <iostream>
# include <string>
# include "Logger.hpp"
# include "Location.hpp"
# include "Webserv.hpp"

class Request ;
class Webserv ;

class Config
{
private:

	std::ifstream						f;		// open index.html
	std::string							_listen;
	std::map<std::string, std::string>	_configMap;
	std::vector<Location> 				_locationVector;
	std::vector<Location> 				_locationExtVector;
	std::vector<Webserv>				_serverVector;
	void								initConfigMap( void );
	void								initLocationMap( std::map<std::string, std::string> & newLoc, std::string path);
	void								addConfigToLocation(std::map<std::string, std::string> newLoc, bool isExtension);
	void								checkFile( std::string fileName );
	bool								checkSemiColon( std::string str );
	void								createServerMap( void );
	void								newLocationConfig( std::string path );

public:

	Config( void );								//default constructor
	// Config( std::string name );					//constructor
	Config( Config const & src);  				//copy
	~Config( void );							//destructor
	Config & operator=( Config const & rhs );	//overload operator =

	void								parseFile( std::string fileName );
	std::map<std::string, std::string>	getConfigMap( void );
	std::vector<Location>				getLocationVector( void );
	std::vector<Webserv> 				getServerVector(void);
};

#endif
