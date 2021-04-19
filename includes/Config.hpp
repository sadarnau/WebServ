#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <fstream>
# include <sstream>
# include <map>
# include <iostream>
# include "Logger.hpp"

class Config
{
private:

	std::ifstream						f;		// open index.html
	std::map<std::string, std::string>	fileMap;
	std::map<std::string, std::string>	_locationMap;

public:

	Config( void );								//default constructor
	// Config( std::string name );					//constructor
	Config( Config const & src);  				//copy
	~Config( void );							//destructor
	Config & operator=( Config const & rhs );	//overload operator =

	void								parseFile( std::string fileName );
	void								checkFile( std::string fileName );
	void								createMap( void );
	void								locationConfig( void );
	void								putInLocMap( std::string line );
	void								errorPageConfig( std::string line );
	void								parseConf( std::string line );
	std::map<std::string, std::string>	getMap( void );
};

#endif
