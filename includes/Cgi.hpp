#ifndef CGI_HPP
# define CGI_HPP

# include <string>
# include <map>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>
# include "Request.hpp"


class Request ;

class Cgi
{
	private:
		Request								*_req;
		std::map<std::string, std::string>	_env;
		char								**_envC;
		char								**_argC;
		std::string							_result;
		std::map<std::string, std::string>	_headers;
		
		void								_initEnv(void);
		void								_initArgC(void);
		char								**_envToCArray(void);
		void								_closeFd(FILE *fIn, FILE *fOut, int fdIn, int fdOut);


	public:
		// Cgi(Request *req, Location *loc);
		Cgi(Request *req);
		Cgi( Cgi const & src );  				//copy
		~Cgi( void );							//destructor
		Cgi & operator=( Cgi const & rhs );		//overload operator =

		bool								processCgi(void);
		std::string							getResult(void);
		void								logCgi(void);
		void								parseResponse(void);
		std::map<std::string, std::string>	getCgiHeaders(void);
};

#endif