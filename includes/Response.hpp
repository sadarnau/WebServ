#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <iostream>
# include <sstream>
# include <fstream>
# include "Logger.hpp"
# include "Webserv.hpp"


class Response
{
private:

	std::string	response;

public:

	Response( void );								//default constructor
	// Response( std::string name );					//constructor
	Response( Response const & src);  				//copy
	~Response( void );								//destructor
	Response & operator=( Response const & rhs );	//overload operator =

	void		fillResponse( void );
	std::string	getResponse( void );
};

#endif
