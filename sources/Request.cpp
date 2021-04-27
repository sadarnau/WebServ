#include "Request.hpp"

////////////////////
// COPLIEN'S
////////////////////
Request::Request( void )
{
	return ;
}

Request::Request(vlocation *locationVector, int inSock, char *buff ) : _inSocket(inSock)
{
	this->_locationVector = locationVector;
	std::string	tmp(buff);	//convert char* to std::string
	this->_buff = tmp;
	this->parseRequest(this->_buff);
	this->selectLocation();
	this->parseUrl();
	this->createPath();
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


////////////////////
// PARSE
////////////////////
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
			this->_target = value;
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


////////////////////
// UTILS
////////////////////

void			Request::updateTarget(std::string target)
{
	this->_target = target;
	this->createPath();
}

void	Request::selectLocation()
{
	// iter through locations
	for (vlocation::iterator it = this->_locationVector->begin() ; it != this->_locationVector->end(); ++it)
    {
		std::map<std::string, std::string> tmpMap(*it);
				
		// compare location["path"] to target on lacation["path"] length
		if (std::strncmp(tmpMap["path"].c_str(), this->_target.c_str(), tmpMap["path"].size()) == 0) 
		{
			// check if its a valid route (for now, if its a correct folder), "/" is always valid
			// relevant if this->target = "/weeeee" and loc["path"] = "/we" for example (/we is not a good loc)
			if (tmpMap["path"] == "/" || this->_target[tmpMap["path"].length()] == '\0' || this->_target[tmpMap["path"].length()] == '/')
			{
				//if no location, add anyone, if theres something, check if the new match on more char than the previous one
				if(this->_selectedLocation.empty())
					this->_selectedLocation = tmpMap;
				else
					if(this->_selectedLocation["path"].size() < tmpMap["path"].size())
						this->_selectedLocation = tmpMap;
			}
		}
	}

	this->_urlTargetPath = this->_target;
	
	// delete location in target (ex: if location is /salut and target /salut/index.html, target become /index.html) - not in case of default loc /
	if (this->_selectedLocation["path"] != "/")
		this->_target = this->_target.substr(this->_selectedLocation["path"].size(), this->_target.size());
	
	// format target
	if(this->_target.front() != '/')
		this->_target.insert(0, "/");
}

void	Request::parseUrl()
{
	int i;

	//Search for query
	if((i = this->_target.find("?")) != -1)
	{
		//query separator is found
		this->_queryString = this->_target.substr(i + 1, this->_target.size() - 2); // i + 1 to skip &, so size - (1 + '&')
		this->_target = this->_target.substr(0, i);
	}
}


void	Request::createPath()
{
	//Create absolute path
	if (this->_selectedLocation["root"].front() == '/')
		this->_absoluteTargetPath =  safeUrlJoin(this->_selectedLocation["root"], this->_target);
	else{
		char cwd[1000];
		getcwd(cwd, sizeof(cwd));
		std::string currentdir = cwd;
		this->_absoluteTargetPath = safeUrlJoin(currentdir, this->_selectedLocation["root"]);
		this->_absoluteTargetPath = safeUrlJoin(this->_absoluteTargetPath, this->_target);
	}
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

void	Request::printRequest( void )
{
	std::ostringstream oss;
	
	oss << "\n----------\nREQUEST OBJECT :\n\n" ;
	oss << std::setw(30) << "request->method" << " : " << this->_method << std::endl;
	oss << std::setw(30) << "request->target" << " : " << this->_target << std::endl;
	oss << std::setw(30) << "request->query" << " : " << this->_queryString << std::endl;
	oss << std::setw(30) << "request->urlTargetPath " << " : " << this->_urlTargetPath << std::endl;
	oss << std::setw(30) << "request->absoluteTargetPath " << " : " << this->_absoluteTargetPath << std::endl;
	oss << std::setw(30) << "selectedLocation[\"path\"] " << " : " << this->_selectedLocation["path"] << std::endl << std::endl;
	oss << "Content of request->headers :" << std::endl << std::endl; 
	oss << std::setw(20) << "KEY" << " : " << "VALUE" << std::endl << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
	{
		oss << std::setw(20) << it->first << " : " << it->second << std::endl;
	}
	oss << std::endl << std::endl;
	Logger::Write(Logger::DEBUG, std::string(BLU), oss.str(), true);

	oss.clear();
	
	//SKIPPED HEADERS
	oss << std::endl << "Skipped headers : ";
	for (std::vector<std::string>::iterator it = this->_skippedHeaders.begin(); it != this->_skippedHeaders.end(); ++it)
		oss << " " << *it;
	oss << std::endl << std::endl;
	
	// RAW REQUEST
	oss << "Raw request :" << std::endl << std::endl;
	oss << this->_buff;
	oss << "----------\n\n";
	oss << RESET;
	Logger::Write(Logger::MORE, std::string(BLU), oss.str(), true);

	return ;
}


////////////////////
// GETTERS / SETTERS
////////////////////
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
	return (this->_target);
}

std::string		Request::getUrlTargetPath()
{
	return (this->_urlTargetPath);
}

std::string		Request::getAbsoluteTargetPath()
{
	return (this->_absoluteTargetPath);
}

std::string		Request::getQueryString()
{
	return (this->_queryString);
}



std::map<std::string, std::string> Request::getSelectedLocation()
{
	return (this->_selectedLocation);
}

std::map<std::string, std::string>	Request::getHeaders()
{
	return(this->_headers);
}

std::ostream &	operator<<(std::ostream & o, Request & rhs)
{
	o << "In this request we have :\n";
	o << "Absolute target path : " << rhs.getAbsoluteTargetPath() << "\n";
	// o << "Relative target path : " << rhs.getRelativeTargetPath() << "\n";
	o << "Incomming socket : " << rhs.getInSock() << "\n";
	o << "Methods : " << rhs.getMethod() << "\n";
	o << "Target : " << rhs.getTarget() << "\n\n";
	o << "Query : " << rhs.getQueryString() << "\n\n";

	return ( o );
}
