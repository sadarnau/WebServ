#include "Config.hpp"

Config::Config( void )
{
	return ;
}

// Config::Config( std::string name )
// {
// 	return ;
// }

Config::Config( Config const & src )
{
	*this = src;
	return ;
}

Config::~Config( void )
{
	return ;
}

Config & Config::operator=( Config const & rhs)
{
	// this->??? = rhs.???;
	(void)rhs;
	return ( *this );
}

void	Config::parseFile( std::string fileName )
{
	checkFile(fileName);
	createMap();
	return ;
}

void	Config::checkFile( std::string fileName )
{
	std::string::size_type	n;
	std::string				line;

	n = fileName.find(".conf");
	if (n == std::string::npos)
		throw (std::exception());	//exeption a faire, pas de .conf

	this->f.open(fileName.c_str());	// open conf file

	if (!f.good())
		throw (std::exception());	//exeption a faire, probleme a l'ouverture

	std::getline(this->f, line);
	if (line.compare("server {"))
		throw (std::exception());	//exeption a faire, conf non valide

	return ;
}

void	Config::createMap( void )
{
	std::string line;

	while (std::getline(this->f, line))
	{
  		if ((line.find("location {")) != std::string::npos )
				locationConfig();
		else if ((line.find("error_page")) != std::string::npos )
			errorPageConfig( line );
		else if (line.size() == 0)
			;
		else if (!line.compare("}"))
			break ;
		else
			parseConf( line );
	}

	return ;
}

void	Config::locationConfig( void )
{
	//create a location class

	return ;
}

void	Config::errorPageConfig( std::string line )
{
	//create an eror map
	(void)line;
	return ;
}

void	Config::parseConf( std::string line )
{
	std::string	value;
    std::string	key;

	std::stringstream ss(line);
	ss >> key >> value;										// set the variables  
	if(ss.fail())											// if value extraction failed, break while loop
		return ;
	this->fileMap[key] = value.substr(0, value.size() - 1);	//-1 to take off the ';'

	return ;
}

std::map<std::string, std::string>	Config::getMap( void )
{
	return (this->fileMap);
}
