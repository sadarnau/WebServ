#ifndef CGI_HPP
# define CGI_HPP

# include <string>
# include <map>
# include <unistd.h>
# include "Request.hpp"

class Request ;

class Cgi
{
	private:
		std::map<std::string, std::string>	_env;
		char								**_envC;

		void								_initEnv();
		char								**_envToCArray();
		Request								*_req;
		std::string							_result;
		
		void								logCgi();

	public:
		// Cgi(Request *req, Location *loc);
		Cgi(Request *req);
		Cgi( Cgi const & src );  				//copy
		~Cgi( void );							//destructor
		Cgi & operator=( Cgi const & rhs );		//overload operator =

		bool		processCgi(std::string body);
		std::string	getResult();

};

#endif