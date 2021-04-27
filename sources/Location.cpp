#include "Location.hpp"

Location::Location(void)
{
	this->_isSet = false;
}

Location::Location(std::map<std::string, std::string> locationMap) : _listen(""), _path(""), _serverName(""), _clientMaxBodySize(""), _root(""), _cgi(""), _autoindex("")
{
	this->_isSet = true;
	this->_index.clear();
	this->_acceptedMethod.clear();
	this->_errorPage.clear();
	for (std::map<std::string, std::string>::const_iterator it = locationMap.begin(); it != locationMap.end(); ++it)
	{
		if (!it->first.compare("listen"))
			this->_listen = it->second;
		else if (!it->first.compare("path"))
			this->_path = it->second;
		else if (!it->first.compare("server_name"))
			this->_serverName = it->second;
		else if (!it->first.compare("client_max_body_size"))
			this->_clientMaxBodySize = it->second;
		else if (!it->first.compare("root"))
			this->_root = it->second;
		else if (!it->first.compare("autoindex"))
			this->_autoindex = it->second;
		else if (!it->first.compare("cgi"))
			this->_cgi = it->second;
		else if (!it->first.compare("index"))
			this->_index = concatToVector(it->second);
		else if (!it->first.compare("accepted_method") && it->second.compare(""))
			this->_acceptedMethod = concatToVector(it->second);
		else
			this->_errorPage[it->first] = it->second;
	}
}

Location::Location(const Location &rhs)
{
	*this = rhs;
}

Location::~Location(void)
{
}

Location &Location::operator=(const Location &rhs)
{
	this->_listen = rhs._listen;
	this->_path = rhs._path;
	this->_serverName = rhs._serverName;
	this->_clientMaxBodySize = rhs._clientMaxBodySize;
	this->_root = rhs._root;
	this->_cgi = rhs._cgi;
	this->_autoindex = rhs._autoindex;
	this->_index = rhs._index;
	this->_acceptedMethod = rhs._acceptedMethod;
	this->_errorPage = rhs._errorPage;

	return (*this);
}

std::string		Location::getListen(void)
{
	return (this->_listen);
}

std::string		Location::getPath(void)
{
	return (this->_path);
}

std::string		Location::getServerName(void)
{
	return (this->_serverName);
}

std::string		Location::getClientMaxBodySize(void)
{
	return (this->_clientMaxBodySize);
}

std::string		Location::getRoot(void)
{
	return (this->_root);
}

std::string		Location::getCgi(void)
{
	return (this->_cgi);
}

std::string		Location::getAutoindex(void)
{
	return (this->_autoindex);
}

std::vector<std::string>			Location::getIndex(void)
{
	return (this->_index);
}

std::vector<std::string>			Location::getAcceptedMethod(void)
{
	return (this->_acceptedMethod);
}

std::map<std::string, std::string>	Location::getErrorPage(void)
{
	return (this->_errorPage);
}

bool								Location::isSet(void)
{
	return (this->_isSet);
}