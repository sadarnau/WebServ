#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <string>
# include <vector>
# include <map>
# include "Utils.hpp"

class Location
{
	private:
		bool								_isSet;
		std::string							_listen;
		std::string							_path;
		std::string							_serverName;
		std::string							_clientMaxBodySizeize;
		std::string							_root;
		std::string							_autoindex;
		std::vector<std::string>			_index;
		std::vector<std::string>			_acceptedMethod;
		std::map<std::string, std::string>	_errorPage;

	public:
		Location(void);
		Location(std::map<std::string, std::string> locationMap);
		Location(const Location &rhs);
		~Location(void);
		Location &operator=(const Location &rhs);

		std::string							getListen(void);
		std::string							getPath(void);
		std::string							getServerName(void);
		std::string							getClientMaxBodySize(void);
		std::string							getRoot(void);
		std::string							getAutoindex(void);
		std::vector<std::string>			getIndex(void);
		std::vector<std::string>			getAcceptedMethod(void);
		std::map<std::string, std::string>	getErrorPage(void);
		bool								isSet(void);
};


#endif