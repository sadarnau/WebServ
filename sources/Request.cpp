#include "Request.hpp"

Request::Request( void )
{
	return ;
}

Request::Request( int inSock, char *buff ) : _inSocket(inSock)
{
	std::string	tmp(buff);	//convert char* to std::string
	this->_buff = tmp;
	this->parseRequest(this->_buff);
	this->printRequest();
	return ;
}

Request::Request( Request const & src )
{
	*this = src;
	return ;
}

Request::~Request( void )
{
	return ;
}

Request & Request::operator=( Request const & rhs)
{
    this->_buff = rhs._buff;
	this->_inSocket = rhs._inSocket;

	return ( *this );
}

bool	Request::isValidHeader(std::string header)
{
	std::string listOfAcceptedHeaders[18] = {"Accept-Charsets", "Accept-Language", "Allow", "Authorization", "Content-Language",
												"Content-Length", "Content-Location", "Content-Type", "Date", "Host", "Last-Modified",
												"Location", "Referer", "Retry-After", "Server", "Transfer-Encoding", "User-Agent",
												"WWW-Authenticate"};
	std::vector<std::string> acceptedHeaders;
	acceptedHeaders.assign(listOfAcceptedHeaders, listOfAcceptedHeaders + 18);

	for (std::vector<std::string>::iterator it = acceptedHeaders.begin(); it != acceptedHeaders.end(); ++it)
		if (header == *it)
			return (true);

	return (false);
}

bool	Request::isRequestMethod(std::string key)
{
	std::string listOfAcceptedMethods[8] = {"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"}; // see https://tools.ietf.org/html/rfc7231 - RFC 7231
	std::vector<std::string> acceptedMethods;
	acceptedMethods.assign(listOfAcceptedMethods, listOfAcceptedMethods + 8);

	for (std::vector<std::string>::iterator it = acceptedMethods.begin(); it != acceptedMethods.end(); ++it)
		if (key == *it)
			return (true);

	return (false);
}

void	Request::parseRequest(std::string req)
{
	std::string				line;
    std::string				value;
    std::string				key;
	std::istringstream		streamReq(req);

	while (std::getline(streamReq, line))
    {
		std::stringstream	ss(line);
		ss >> key >> value;										// set the variables

		if(ss.fail())											// if value extraction failed, break while loop
			break;

		if (this->isRequestMethod(key))							// handle method line
		{
			this->_method = key;
			this->_relativeTargetPath = value;
		}
		else													// handle anything else (= headers)
		{
			key = key.substr(0, key.length() - 1);				// delete char ':' at the end of key
			if (this->isValidHeader(key))
				this->_headers[key] = value;
			else
				this->_skippedHeaders.push_back(key);
		}
	}
}

void	Request::printRequest( void )
{
	std::ostringstream oss;
	
	oss << "\n----------\nREQUEST OBJECT :\n\n" ;
	oss << std::setw(20) << "request->method" << " : " << this->_method << std::endl;
	oss << std::setw(20) << "request->target " << " : " << this->_relativeTargetPath << std::endl << std::endl;

	oss << "Content of request->headers :" << std::endl << std::endl; 
	oss << std::setw(20) << "KEY" << " : " << "VALUE" << std::endl << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
	{
		oss << std::setw(20) << it->first << " : " << it->second << std::endl;
	}

	//SKIPPED HEADERS
	oss << std::endl << std::endl << "Skipped headers : ";
	for (std::vector<std::string>::iterator it = this->_skippedHeaders.begin(); it != this->_skippedHeaders.end(); ++it)
		oss << " " << *it;
	oss << std::endl << std::endl;

	// RAW REQUEST
	oss << "Raw request :" << std::endl << std::endl;
	oss << this->_buff;
	oss << "----------\n\n";
	oss << RESET;
	Logger::Write(Logger::DEBUG, std::string(BLU), oss.str(), true);
	return ;
}

int		Request::getInSock( void )
{
	return (this->_inSocket);
}

std::string		Request::getMethod()
{
	return (this->_method);
}

std::string		Request::getTarget()
{
	return (this->_relativeTargetPath);
}
