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

void	Config::initConfigMap(void)
{
	this->_configMap["listen"] = "127.0.0.1:8080";
	this->_configMap["server_name"] = "";
	this->_configMap["client_max_body_size"] = "";
}

void	Config::parseFile( std::string fileName )
{
	this->checkFile(fileName);
	this->initConfigMap();
	this->createServerMap();
	printMap(this->_configMap);
	for (std::vector<std::map<std::string, std::string> >::const_iterator it = this->_locationVector.begin(); it != this->_locationVector.end(); ++it)
	{
		//Logger::Write(Logger::DEBUG, )
		printMap(*it);
	}
	return ;
}

void	Config::checkFile( std::string fileName )
{
	std::string::size_type	n;
	std::string				line;

	n = fileName.find(".conf");
	if (n == std::string::npos)
	{
		Logger::Error("Config file must be filename.conf\n");
		throw (std::exception());	//exeption a faire, conf non valide
	}	//exeption a faire, pas de .conf

	this->f.open(fileName.c_str());	// open conf file

	if (!f.good())
	{
		Logger::Error("Fail opening conf file\n");
		throw (std::exception());	//exeption a faire, conf non valide
	}	//exeption a faire, probleme a l'ouverture

	std::getline(this->f, line);
	if (line.compare("server {"))
	{
		Logger::Error("Must start by \"server {\"\n");
		throw (std::exception());	//exeption a faire, conf non valide
	}

	return ;
}

bool	Config::checkSemiColon(std::string str)
{
	if (str[str.size() - 1] == ';')
		return (true);
	return (false);
}

void	Config::createServerMap( void )
{
	std::string line;
	std::vector<std::string> split;
	bool locationFound = false;
	bool listenFound = false;
	bool endOfSectionFound = false;

	while (std::getline(this->f, line))
	{
		splitStringToVector(line, split);
		if (split.empty())
			;
  		else if (!split[0].compare("listen") && split.size() == 2 && this->checkSemiColon(split.back()))
		{
			listenFound = true;
			this->_configMap["listen"] = split[1].substr(0, split[1].size() - 1);
		}
		else if (!split[0].compare("server_name") && split.size() == 2 && this->checkSemiColon(split.back()))
			this->_configMap["server_name"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("client_max_body_size") && split.size() == 2 && this->checkSemiColon(split.back()))
			this->_configMap["client_max_body_size"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("error_page") && split.size() == 3 && this->checkSemiColon(split.back()))
			this->_configMap[split[1]] = split[2].substr(0, split[2].size() - 1);
		else if (!split[0].compare("location") && split.size() == 3 && !split[2].compare("{"))
		{
			locationFound = true;
			this->newLocationConfig(split[1]);
		}
		else if (!split[0].compare("}") && split.size() == 1)
		{
			endOfSectionFound = true;
			break ;
		}
		else
		{
			Logger::Error("Bad conf format\n");
			throw (std::exception());
		}
		split.clear();
	}
	if (!endOfSectionFound)
	{
		Logger::Error("Server section doesn't end by }\n");
		throw (std::exception());
	}
	if (!listenFound)
	{
		Logger::Error("No listen definition\n");
		throw (std::exception());
	}
	if (!locationFound)
	{
		Logger::Error("No location definition\n");
		throw (std::exception());
	}
	return ;
}

void	Config::initLocationMap(std::map<std::string, std::string> & newLoc, std::string path)
{
	newLoc["path"] = path;
	newLoc["accepted_method"] = "all";
	newLoc["root"] = "./";
	newLoc["autoindex"] = "off";
	newLoc["index"] = "";
	newLoc["cgi"] = "";
}

void	Config::newLocationConfig( std::string path )
{
	std::string line;
	std::vector<std::string> split;
	std::map<std::string, std::string> newLoc;
	bool endOfSectionFound = false;

	this->initLocationMap(newLoc, path);
	while (std::getline(this->f, line))
	{
		splitStringToVector(line, split);
		std::ostringstream ss;
		ss << split.size();
		if (split.empty())
			;
  		else if (!split[0].compare("accepted_method") && split.size() == 2 && this->checkSemiColon(split.back()))
			newLoc["accepted_method"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("root") && split.size() == 2 && this->checkSemiColon(split.back()))
			newLoc["root"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("autoindex") && split.size() == 2 && this->checkSemiColon(split.back()))
			newLoc["autoindex"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("index") && split.size() == 2 && this->checkSemiColon(split.back()))
			newLoc["index"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("cgi") && split.size() == 2 && this->checkSemiColon(split.back()))
			newLoc["cgi"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("}") && split.size() == 1)
		{
			this->_locationVector.push_back(newLoc);
			endOfSectionFound = true;
			break ;
		}
		else
		{
			Logger::Error("Bad location format\n");
			if (!this->checkSemiColon(split.back()))
				Logger::Error("Missing a SEMICOLON .... somewhee...\n");
			throw (std::exception());
		}
		split.clear();
	}
	if (!endOfSectionFound)
	{
		Logger::Error("Location section doesn't end by }\n");
		throw (std::exception());
	}
	return ;
}

std::map<std::string, std::string>	Config::getConfigMap( void )
{
	return (this->_configMap);
}

std::vector<std::map<std::string, std::string> >	Config::getLocationVector( void )
{
	return (this->_locationVector);
}