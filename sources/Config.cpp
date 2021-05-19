#include "Config.hpp"

Config::Config(void)
{
	return ;
}

Config::Config(Config const & src)
{
	*this = src;
	return ;
}

Config::~Config(void)
{
	return ;
}

Config & Config::operator=(Config const & rhs)
{
	this->_configMap = rhs._configMap;
	this->_listen = rhs._listen;
	this->_allListen = rhs._allListen;
	this->_locationVector = rhs._locationVector;
	this->_locationExtVector = rhs._locationExtVector;
	this->_serverVector = rhs._serverVector;

	return (*this);
}

void	Config::initConfigMap(void)
{
	this->_configMap["listen"] = "127.0.0.1:8080";
	this->_configMap["server_name"] = "";
	this->_configMap["client_max_body_size"] = "";
	this->_configMap["accepted_method"] = "";
	this->_configMap["root"] = "";
	this->_configMap["autoindex"] = "off";
	this->_configMap["index"] = "";
	this->_configMap["cgi_path"] = "";
	this->_configMap["cgi_ext"] = "";
	this->_configMap["authentication"] = "";

	return ;
}

void	Config::parseFile(std::string fileName)
{
	std::map<std::string, std::string> newLoc;

	this->checkFile(fileName);
	this->initConfigMap();
	this->createServerMap();

	return ;
}

void	Config::checkFile(std::string fileName)
{
	std::string::size_type	n;
	std::string				line;

	n = fileName.find(".conf");
	if (n == std::string::npos)
	{
		Logger::Error("Config file must be filename.conf\n");
		throw (std::exception());
	}
	this->f.open(fileName.c_str());

	if (!f.good())
	{
		Logger::Error("Fail opening conf file\n");
		throw (std::exception());
	}
	std::getline(this->f, line);

	return ;
}

bool	Config::checkSemiColon(std::string str)
{
	if (str[str.size() - 1] == ';')
		return (true);

	return (false);
}

bool	Config::checkMultipleListen(std::string listen)
{
	std::vector<std::string>::const_iterator it(this->_allListen.begin());
	std::vector<std::string>::const_iterator it2(this->_allListen.end());
	for( ; it != it2; it++)
	{
		if (!listen.compare(*it))
			return (true);
	}
	return (false);		
}

void	Config::createServerMap(void)
{
	std::string 						line;
	std::vector<std::string>			split;
	std::map<std::string, std::string>	newLoc;
	bool								locationFound = false;
	bool								serverNameFound = false;
	bool								listenFound = false;
	bool								endOfSectionFound = false;
	bool								inServerConfig = false;
	bool								rootLocationFound = false;

	this->initLocationMap(newLoc, "/");
	while (std::getline(this->f, line))
	{
		Utils::splitStringToVector(line, split);
		if (split.empty() || split[0][0] == '#')
			;
		else if (!split[0].compare("server") && split.size() == 2 && !split[1].compare("{") && !inServerConfig)
		{
			inServerConfig = true;
			endOfSectionFound = false;
			listenFound = false;
			serverNameFound = false;
		}
  		else if (!split[0].compare("listen") && split.size() == 2 && this->checkSemiColon(split.back()) && inServerConfig)
		{
			listenFound = true;
			this->_configMap["listen"] = split[1].substr(0, split[1].size() - 1);
			this->_listen = this->_configMap["listen"];
			if (!this->_allListen.empty() && this->checkMultipleListen(this->_listen))
			{
				Logger::Error("Multiple listen definition\n");
				throw (std::exception());
			}
			this->_allListen.push_back(this->_listen);
		}
		else if (!split[0].compare("server_name") && split.size() == 2 && this->checkSemiColon(split.back()) && inServerConfig)
		{
			this->_configMap["server_name"] = split[1].substr(0, split[1].size() - 1);
			serverNameFound = true;

		}
  		else if (!split[0].compare("accepted_method") && split.size() == 2 && this->checkSemiColon(split.back()) && inServerConfig)
			this->_configMap["accepted_method"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("client_max_body_size") && split.size() == 2 && this->checkSemiColon(split.back()) && inServerConfig)
			this->_configMap["client_max_body_size"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("error_page") && split.size() == 3 && this->checkSemiColon(split.back()) && inServerConfig)
			this->_configMap[split[1]] = split[2].substr(0, split[2].size() - 1);
		else if (!split[0].compare("root") && split.size() == 2 && this->checkSemiColon(split.back()) && inServerConfig)
			this->_configMap["root"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("autoindex") && split.size() == 2 && this->checkSemiColon(split.back()) && inServerConfig)
			this->_configMap["autoindex"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("authentication") && split.size() == 2 && this->checkSemiColon(split.back()) && inServerConfig)
			this->_configMap["authentication"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("index") && split.size() == 2 && this->checkSemiColon(split.back()) && inServerConfig)
			this->_configMap["index"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("cgi_path") && split.size() == 2 && this->checkSemiColon(split.back()) && inServerConfig)
			this->_configMap["cgi_path"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("cgi_ext") && split.size() == 2 && this->checkSemiColon(split.back()) && inServerConfig)
			this->_configMap["cgi_ext"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("location") && split.size() == 3 && !split[2].compare("{") && inServerConfig)
		{
			if (!split[1].compare("/"))
				rootLocationFound = true;
			locationFound = true;
			this->newLocationConfig(split[1]);
		}
		else if (!split[0].compare("}") && split.size() == 1 && inServerConfig)
		{
			inServerConfig = false;
			endOfSectionFound = true;
			if (!listenFound)
			{
				Logger::Error("No listen definition\n");
				throw (std::exception());
			}
			if (!rootLocationFound)
				addConfigToLocation(newLoc, false);
			this->_serverVector.push_back(Webserv(this->_listen, this->_locationVector, this->_locationExtVector));
			newLoc.clear();
			this->initLocationMap(newLoc, "/");
			this->_configMap.clear();
			this->_locationVector.clear();
			this->_locationExtVector.clear();
			this->initConfigMap();
			rootLocationFound = false;
		}
		else
		{
			Logger::Error("Bad conf format\n");
			throw (std::exception());
		}
		split.clear();
	}

	if (inServerConfig && !endOfSectionFound)
	{
		Logger::Error("Server section doesn't end by }\n");
		throw (std::exception());
	}

	if (!locationFound)
	{
		Logger::Error("No location definition\n");
		throw (std::exception());
	}
	if (!serverNameFound)
	{
		Logger::Error("No server name definition\n");
		throw (std::exception());
	}

	return ;
}

void	Config::initLocationMap(std::map<std::string, std::string> & newLoc, std::string path)
{
	newLoc["path"] = path;

	return ;
}

void	Config::addConfigToLocation(std::map<std::string, std::string> newLoc, bool isExtension)
{
	if (isExtension)
	{
		newLoc["cgi_ext"] = newLoc["path"];
		this->_locationExtVector.push_back(Location(newLoc));
	}
	else
	{
		for (std::map<std::string, std::string>::const_iterator it = this->_configMap.begin(); it != this->_configMap.end(); ++it)
			if (!newLoc.count(it->first))
				newLoc[it->first] = it->second;
		this->_locationVector.push_back(Location(newLoc));
	}

	return ;
}

void	Config::newLocationConfig(std::string path)
{
	std::string 						line;
	std::vector<std::string>			split;
	std::map<std::string, std::string>	newLoc;
	bool								endOfSectionFound = false;
	bool								isExtension = false;

	if (path[0] == '*' && path[1] == '.')
	{
		this->initLocationMap(newLoc, path.substr(1));
		isExtension = true;
	}
	else
		this->initLocationMap(newLoc, path);

	while (std::getline(this->f, line))
	{
		Utils::splitStringToVector(line, split);
		std::ostringstream ss;
		ss << split.size();
		if (split.empty() || split[0][0] == '#')
			;
  		else if (!split[0].compare("accepted_method") && split.size() == 2 && this->checkSemiColon(split.back()))
			newLoc["accepted_method"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("root") && split.size() == 2 && this->checkSemiColon(split.back()))
			newLoc["root"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("autoindex") && split.size() == 2 && this->checkSemiColon(split.back()))
			newLoc["autoindex"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("authentication") && split.size() == 2 && this->checkSemiColon(split.back()))
			this->_configMap["authentication"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("index") && split.size() == 2 && this->checkSemiColon(split.back()))
			newLoc["index"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("cgi_path") && split.size() == 2 && this->checkSemiColon(split.back()))
			newLoc["cgi_path"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("cgi_ext") && split.size() == 2 && this->checkSemiColon(split.back()))
			newLoc["cgi_ext"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("client_max_body_size") && split.size() == 2 && this->checkSemiColon(split.back()))
			newLoc["client_max_body_size"] = split[1].substr(0, split[1].size() - 1);
		else if (!split[0].compare("error_page") && split.size() == 3 && this->checkSemiColon(split.back()))
			newLoc[split[1]] = split[2].substr(0, split[2].size() - 1);
		else if (!split[0].compare("}") && split.size() == 1)
		{
			addConfigToLocation(newLoc, isExtension);
			endOfSectionFound = true;
			break ;
		}
		else
		{
			Logger::Error("Bad location format\n");
			if (!this->checkSemiColon(split.back()))
				Logger::Error("Missing a SEMICOLON .... somewhere...\n");
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

std::map<std::string, std::string>	Config::getConfigMap(void)
{
	return (this->_configMap);
}

std::vector<Webserv> 				Config::getServerVector(void)
{
	return (this->_serverVector);
}

std::vector<Location>				Config::getLocationVector(void)
{
	return (this->_locationVector);
}
