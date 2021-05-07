#include "Request.hpp"

////////////////////
// COPLIEN'S
////////////////////
Request::Request(vlocation *locationVector, int inSock, std::string buff)
{
	this->_inSocket = inSock;
	this->_locationVector = locationVector;
	this->_buff = buff;

	this->parseRequest(this->_buff);
	this->selectLocation();
	this->parseUrl();
	this->createPath();
	return ;
}

Request::Request(Request const & src)
{
	*this = src;
	return ;
}

Request::~Request(void)
{
	return ;
}

Request & Request::operator=(Request const & rhs)
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
	std::string				header;
	std::string				body;

	header = req.substr(0, req.find("\r\n\r\n"));
	body = req.substr(req.find("\r\n\r\n") + 4, req.length());

	// parse header
	std::string				line;
    std::string				key;
    std::string				value;
    std::string				message;
	std::istringstream		streamHeader(header);

	// handle first line
	std::getline(streamHeader, line);
	std::stringstream	ss(line);
	ss >> key >> value >> message;										
	this->_method = key;
	this->_target = value;
	this->_message = message;

	// handle rest of request
	while (std::getline(streamHeader, line))
    {
		std::stringstream	ss(line);
		ss >> key >> value;										// set the variables

		if(ss.fail())											// if value extraction failed, break while loop
			break;

		key = key.substr(0, key.length() - 1);				// delete char ':' at the end of key
		if (this->isValidHeader(key))
			this->_headers[key] = value;
		else
			this->_skippedHeaders.push_back(key);
	}

	this->_body = body;
}


////////////////////
// UTILS
////////////////////
void	Request::selectLocation(void)
{
	// iter through locations
	for (vlocation::iterator it = this->_locationVector->begin() ; it != this->_locationVector->end(); ++it)
    {
		Location tmpLoc(*it);
				
		// compare location["path"] to target on lacation["path"] length
		if (std::strncmp(tmpLoc.getPath().c_str(), this->_target.c_str(), tmpLoc.getPath().size()) == 0) 
		{
			// check if its a valid route (for now, if its a correct folder), "/" is always valid
			// relevant if this->target = "/weeeee" and loc["path"] = "/we" for example (/we is not a good loc)
			if (tmpLoc.getPath() == "/" || this->_target[tmpLoc.getPath().length()] == '\0' || this->_target[tmpLoc.getPath().length()] == '/')
			{
				//if no location, add anyone, if theres something, check if the new match on more char than the previous one
				if(!this->_selectedLocation.isSet())
					this->_selectedLocation = tmpLoc;
				else
					if(this->_selectedLocation.getPath().size() < tmpLoc.getPath().size())
						this->_selectedLocation = tmpLoc;
			}
		}
	}
	this->_urlTargetPath = this->_target;

	// delete location in target (ex: if location is /salut and target /salut/index.html, target become /index.html) - not in case of default loc /
	if (this->_selectedLocation.getPath() != "/")
		this->_target = this->_target.substr(this->_selectedLocation.getPath().size(), this->_target.size());
	
	// format target
	if(this->_target.front() != '/')
		this->_target.insert(0, "/");
}

void	Request::parseUrl(void)
{
	int i;

	//Search for query
	if((i = this->_target.find("?")) != -1)
	{
		//query separator is found
		this->_queryString = this->_target.substr(i + 1, this->_target.size() - 2); // i + 1 to skip &, so size - (1 + '&')
		this->_target = this->_target.substr(0, i);

		this->_urlTargetPath = this->_urlTargetPath.substr(0, this->_urlTargetPath.find("?"));
	}
}

void			Request::updateTarget(std::string target)
{
	this->_target = target;
	this->createPath();
}

void	Request::createPath(void)
{
	//Create absolute path
	if (this->_selectedLocation.getRoot().front() == '/')
		this->_absoluteTargetPath =  safeUrlJoin(this->_selectedLocation.getRoot(), this->_target);
	else{
		char cwd[1000];
		getcwd(cwd, sizeof(cwd));
		std::string currentdir = cwd;
		this->_absoluteTargetPath = safeUrlJoin(currentdir, this->_selectedLocation.getRoot());
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

////////////////////
// GETTERS / SETTERS
////////////////////
int		Request::getInSock(void)
{
	return (this->_inSocket);
}

std::string		Request::getMethod(void)
{
	return (this->_method);
}

std::string		Request::getBody(void)
{
	return (this->_body);
}

std::string		Request::getTarget(void)
{
	return (this->_target);
}

std::string		Request::getUrlTargetPath(void)
{
	return (this->_urlTargetPath);
}

std::string		Request::getAbsoluteTargetPath(void)
{
	return (this->_absoluteTargetPath);
}

std::string		Request::getQueryString(void)
{
	return (this->_queryString);
}

Location		Request::getSelectedLocation(void)
{
	return (this->_selectedLocation);
}

std::map<std::string, std::string>	Request::getHeaders(void)
{
	return(this->_headers);
}

////////////////////
// LOG
////////////////////
void	Request::logRequest(int serverNbr)
{
	Logger::Write(Logger::INFO, BLU, "server[" + std::to_string(serverNbr) +
		"] : request received [method: " + this->getMethod() + "] [location: " +
		this->getSelectedLocation().getPath() + "] [target: " + this->getTarget() + "]");
	
	std::ostringstream oss;
	
	oss << "request :\n\n" ;
	oss << std::setw(30) << "request->method" << " : " << this->_method << std::endl;
	oss << std::setw(30) << "request->target" << " : " << this->_target << std::endl;
	oss << std::setw(30) << "request->query" << " : " << this->_queryString << std::endl;
	oss << std::setw(30) << "request->body" << " : " << this->_body << std::endl;
	oss << std::setw(30) << "request->urlTargetPath " << " : " << this->_urlTargetPath << std::endl;
	oss << std::setw(30) << "request->absoluteTargetPath " << " : " << this->_absoluteTargetPath << std::endl;
	oss << std::setw(30) << "selectedLocation.getPath() " << " : " << this->_selectedLocation.getPath() << std::endl << std::endl;
	oss << "Content of request->headers :" << std::endl << std::endl; 
	for (std::map<std::string, std::string>::const_iterator it = this->_headers.begin(); it != this->_headers.end(); ++it)
	{
		oss << std::setw(20) << it->first << " : " << it->second << std::endl;
	}
	Logger::Write(Logger::DEBUG, WHT, oss.str());

	std::ostringstream oss2;

	//SKIPPED HEADERS
	oss2 << std::endl << "Skipped headers : ";
	for (std::vector<std::string>::iterator it = this->_skippedHeaders.begin(); it != this->_skippedHeaders.end(); ++it)
		oss2 << " " << *it;
	oss2 << std::endl << std::endl;
	
	// RAW REQUEST
	oss2 << "Raw request :" << std::endl << std::endl;
	oss2 << this->_buff;
	oss2 << "\n";
	oss2 << RESET;
	Logger::Write(Logger::MORE, BLU, oss2.str());

	return ;
}