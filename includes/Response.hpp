#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <iostream>
# include <sstream>
# include <fstream>
# include <sys/types.h>
# include <dirent.h>
# include "Logger.hpp"
# include "Webserv.hpp"
# include "Request.hpp"
# include "Utils.hpp"

class Request ;

class Response
{
private:
	Request									*_req;
	int										_socket;

	std::map<std::string, std::string> 		_location;

	std::string 							_httpVersion;
	int										_responseCode;
	std::string								_responseCodeMessage;

	std::string								_contentType;
	std::map<std::string, std::string>		_headers;

	std::string								_header;
	std::string								_body;
	std::string								_response;

public:
	Response( Request *req, int socket );	//default constructor
	Response( Response const & src );  					//copy
	~Response( void );									//destructor
	Response & operator=( Response const & rhs );		//overload operator =

	void			send();
	void			buildHeader();
	void			buildResponse();

	void			processGet();
	void			processPost();

	bool			autoIndexResponse();
	std::string		getIndexTarget();
	void			setToErrorPage(int errorNumber);

	bool			isIndexPagePresent();
	bool			isDirectory();
	std::string		getContentType(std::string target);
	
	void			setHeaders( int responseCode, std::string responseCodeMessage, std::string contentType );
	std::string		getResponse();
	std::string		getBody();
	std::string		getHeader();


};

std::ostream &	operator<<(std::ostream & o, Response & rhs);

#endif
