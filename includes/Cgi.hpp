#ifndef CGI_HPP
# define CGI_HPP

# include <string>
# include <map>
# include <unistd.h>
# include "Request.hpp"

class Cgi
{
	private:
		std::map<std::string, std::string>	_env;
		char								**_envC;

		void								_initEnv();
		char								**_envToCArray();
		Request								*_req;

	public:
		// Cgi(Request *req, Location *loc);
		Cgi(Request *req);
		Cgi( Cgi const & src );  				//copy
		~Cgi( void );							//destructor
		Cgi & operator=( Cgi const & rhs );		//overload operator =
};

#endif