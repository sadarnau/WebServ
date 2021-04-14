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

	// search from beginning of string
	n = fileName.find(".conf");
	if (n == std::string::npos)
		throw (std::exception()); //exeption a faire, pas de .conf
	
	this->f.open(fileName.c_str()); // open index.html

	if (!f.good())
		throw (std::exception()); //exeption a faire, probleme a l'ouverture

	return ;
}

void	Config::createMap( void )
{
	std::string line;
    std::string	value;
    std::string	key;

	while (std::getline(this->f, line))
	{
		std::stringstream ss(line);
	    ss >> key >> value;					// set the variables  
		this->fileMap[key] = value;
		std::cout << key << " = " << value << "\n";
	}

	return ;
}
