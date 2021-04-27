#ifndef CGI_HPP
# define CGI_HPP

# include <string>
# include <map>

class Cgi
{
	private:
		std::map<std::string, std::string>	_env;
		char								**_envC;

		void								_initEnv();
		char								**_envToCArray();

	public:
		Cgi();
		Cgi( Cgi const & src );  				//copy
		~Cgi( void );							//destructor
		Cgi & operator=( Cgi const & rhs );		//overload operator =
};

#endif