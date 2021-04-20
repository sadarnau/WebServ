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
  		if ((line.find("location / {")) != std::string::npos )
				locationConfig();
		else if ((line.find("error_page")) != std::string::npos )
			;
		else if (line.empty())
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
	std::string line;

	while (std::getline(this->f, line))
	{
		int	begining = line.find_first_not_of("\t ");
  		if (line.empty())
			;
		else if (!line.compare(begining, 5, "index") || !line.compare(begining, 4, "root") || 
		  !line.compare(begining, 20, "client_max_body_size") || !line.compare(begining, 9, "autoindex"))
		  	putInLocMap(line);
		else if (!line.compare(begining, 10, "error_page"))
		  	putInLocMap(line.substr(line.find_first_not_of("\t ", begining + 10)));
		else if (!line.compare(begining, 1, "}"))
			break ;
		else
			throw (std::exception());
	}

	return ;
}

void	Config::putInLocMap( std::string line )
{
	std::string	value;
    std::string	key;

	std::stringstream ss(line);
	ss >> key >> value;													// set the variables  
	if(ss.fail())														// if value extraction failed, break while loop
		return ;
	if (value[value.size() - 1] == ';')
		this->_locationMap[key] = value.substr(0, value.size() - 1);	//-1 to take off the ';'
	else
		throw (std::exception());										//Create an exeption, conf file not good

	return ;
}

void	Config::parseConf( std::string line )
{
	std::string	value;
    std::string	key;

	std::stringstream ss(line);
	ss >> key >> value;											// set the variables  
	if(ss.fail())												// if value extraction failed, break while loop
		return ;
	if (value[value.size() - 1] == ';')
		this->fileMap[key] = value.substr(0, value.size() - 1);	//-1 to take off the ';'
	else
		throw (std::exception());								//Create an exeption, conf file not good

	return ;
}

std::map<std::string, std::string>	Config::getMap( void )
{
	return (this->fileMap);
}
