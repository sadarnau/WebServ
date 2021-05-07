#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <iostream>
# include <sstream>
# include <fstream>
# include <algorithm>
# include <sys/types.h>
# include <dirent.h>
# include <unistd.h>
# include "Logger.hpp"
# include "Webserv.hpp"
# include "Request.hpp"
# include "Utils.hpp"
# include "Location.hpp"
# include "Cgi.hpp"

class Request ;

class Response
{
private:
	Request									*_req;
	int										_socket;
	Location 								_location;
	std::string 							_httpVersion;
	int										_responseCode;
	std::string								_contentType;
	std::map<std::string, std::string>		_headers;
	std::string								_header;
	std::string								_body;
	std::string								_response;
	std::map<int, std::string>				_responseMessages;
	bool									_isSetToError;

	void			send(void);
	void			buildHeader(void);
	void			buildResponse(void);

	void			processGet(void);
	void			processPost(void);
	void			processPut(void);
	void			processOptions(void);
	void			processDelete(void);
	void			processTrace(void);

	bool			autoIndexResponse(void);
	std::string		getIndexTarget(void);
	bool			isIndexPagePresent(void);

	void			checkErrors(void);
	void			initResponseMessageMap(void);
	void			setToErrorPage(int errorNumber);
	std::string		generateDefaultErrorPage(std::string errorNbr, std::string message);

	bool			isDirectory(void);
	std::string		getContentType(std::string target);
	bool			isValidMethod(std::string key);
	bool			isValidHttpMethod(std::string key);

	void			setResponseCode(int responseCode);
	void			setBody(std::string body);
	void			setContentType(std::string contentType);

public:
	Response( Request *req, int socket );	//default constructor
	Response( Response const & src );  					//copy
	~Response( void );									//destructor
	Response & operator=( Response const & rhs );		//overload operator =

	std::string		getResponse(void);
	std::string		getBody(void);
	std::string		getHeader(void);
	std::string		getResponseCodeStr(void);
	int				getResponseCode(void);
	std::string		getResponseCodeMessage(void);
	std::string		getContentLength(void);

	void			logResponse(int serverNbr);
};

#endif
