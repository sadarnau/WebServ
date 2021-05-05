#include "Response.hpp"

////////////////////
// Coplien's
////////////////////
Response::Response(Request *req, int socket)
{
	this->initResponseMessageMap();

	this->_isSetToError = false;
	this->_location = req->getSelectedLocation();
	this->_req = req;
	this->_socket = socket;
	this->_httpVersion = "HTTP/1.1";

	this->buildResponse();
	this->send();
}

Response::Response( Response const & src )
{
	*this = src;
	return ;
}

Response::~Response( void )
{
	return ;
}

Response & Response::operator=( Response const & rhs)
{
	(void)rhs;
	return ( *this );
}


////////////////////
// METHODS
////////////////////
void	Response::send()
{
	write(this->_socket, this->_response.c_str() , this->_response.length());	//to protect
}

void	Response::buildResponse()
{
	std::string		requestMethod = this->_req->getMethod();

	// METHODS
	if (!this->isValidMethod(requestMethod))
	{
		this->setToErrorPage(405);
		return ;
	}

	if (requestMethod == "GET")
		this->processGet();
	else if (requestMethod == "POST")
		this->processPost();
	else if (requestMethod == "PUT")
		this->processOption();
	else if (requestMethod == "TRACE")
		this->processTrace();
	else if (requestMethod == "OPTIONS")
		this->processTrace();
	else if (requestMethod == "DELETE")
		this->processDelete();

	// CGI
	if (!this->_location.getCgi().empty())
	{
		Cgi		cgi(this->_req);

		if(cgi.processCgi(this->_body))
			this->setBody(cgi.getResult());
		else
			this->setToErrorPage(500);
	}

	// BUILD HEADER AND RESPONSE
	this->buildHeader();
	this->_response = this->_header + this->_body;
}

void	Response::buildHeader()
{
	std::ostringstream header;

	header << this->_httpVersion << " " << this->_responseCode << " " << this->_responseMessages[this->_responseCode] << "\r\n";

	this->_headers["Content-Type"] = this->_contentType;
	this->_headers["Content-Length"] = std::to_string(this->_body.size());
	this->_headers["Server"] = std::string("Webserv");
	this->_headers["Date"] = getDate();

	std::map<std::string, std::string> tmpHeaders = this->_headers;

	for (std::map<std::string, std::string>::const_iterator it = tmpHeaders.begin(); it != tmpHeaders.end(); it++)
		header << it->first << ": " << it->second << "\r\n";

	header << "\r\n";						//End of header

	this->_header = header.str();

	Logger::Write(Logger::DEBUG, BLU, "response : header\n\n" + this->_header + "\n-------\n");
}


////////////////////
// HTTP METHODS
////////////////////
void	Response::processGet()
{
	std::string		auto_index = this->_location.getAutoindex();

	// Directory Request
	if (this->isDirectory())
	{
		if (this->isIndexPagePresent())
			this->_req->updateTarget(this->getIndexTarget());
		else if(auto_index == "on" && this->autoIndexResponse())  //autoIndexResponse return true on success
				return ;
	}
	this->checkErrors();

	// Here comes the block where you check the file ext and define content_type
	// maybe this is called too soon in process
	this->setContentType(this->getContentType(this->_req->getTarget()));

	// Check if the file can be open and create response
	std::ifstream 	f(this->_req->getAbsoluteTargetPath().c_str()); // open file

	if (f.good())
	{
		this->setResponseCode(200);
		std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>()); //initialize str with index.html content
		this->setBody(str);
	}
	this->checkErrors();
	f.close();
}


void	Response::processPost()
{
	std::string		auto_index = this->_location.getAutoindex();

	// Directory Request
	if (this->isDirectory())
	{
		if (this->isIndexPagePresent())
			this->_req->updateTarget(this->getIndexTarget());
		else if(auto_index == "on" && this->autoIndexResponse())  //autoIndexResponse return true on success
				return ;
	}
	this->checkErrors();

	this->setContentType(this->getContentType(this->_req->getTarget()));

	// Check if the file can be open and create response
	std::ifstream 	f(this->_req->getAbsoluteTargetPath().c_str()); // open file

	if (f.good())
	{
		this->setResponseCode(200);
		std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>()); //initialize str with index.html content
		this->setBody(str);
	}
	this->checkErrors();
	f.close();
}

void	Response::processPut(void)
{
	//std::string toWrite(this->_req.getBody());


}

void	Response::processOption()
{
	std::string allow;
	std::vector<std::string> acceptedMethods = this->_location.getAcceptedMethod();

	if (acceptedMethods.empty())
		allow = std::string("GET, HEAD, POST, PUT, TRACE, OPTIONS, DELETE");
	else
	{
		for (std::vector<std::string>::const_iterator it = acceptedMethods.begin(); it != acceptedMethods.end(); it++)
		{
			allow += *it;
			if (it != acceptedMethods.end() - 1)
				allow += ", ";
		}
	}

	this->_headers["Allow"] = allow;
	this->setResponseCode(200);
}

void	Response::processTrace(void)
{
	this->setResponseCode(200);
	this->setContentType("message/html");
}

void	Response::processDelete(void)
{
	std::ifstream	f(this->_req->getAbsoluteTargetPath());

	std::cout << "\n\npath : " << this->_req->getAbsoluteTargetPath().c_str() << "\n\n";
	if (f.good())
	{
		std::cout << "\n\npath : " << this->_req->getAbsoluteTargetPath().c_str() << "\n\n";
		if (std::remove(this->_req->getAbsoluteTargetPath().c_str()) == 0)
			this->setResponseCode(204);
		else
			this->setResponseCode(202);
	}
	else
		this->checkErrors();
}


////////////////////
// AUTO INDEX
////////////////////
bool		Response::autoIndexResponse()
{
	DIR *directory;
	struct dirent *dircontent;

	if ((directory = opendir(this->_req->getAbsoluteTargetPath().c_str())))
	{
		this->setResponseCode(200);
		this->setContentType("text/html");

		std::ifstream content_start("files/default_pages/auto_index_start.html");
		std::ostringstream content;
		std::ifstream content_end("files/default_pages/auto_index_end.html");

		content << std::string((std::istreambuf_iterator<char>(content_start)), std::istreambuf_iterator<char>());

		content << "<h1>Directory : " << this->_req->getUrlTargetPath() << "</h1>";
		content << "<ul>" << std::endl;

		while((dircontent = readdir(directory)))
		{
				content << "<li>" << "<a href='" << safeUrlJoin(this->_req->getUrlTargetPath(), dircontent->d_name) << 
				"'>" << dircontent->d_name << "</a>" << "</li>" << std::endl;
		}
		content << "</ul>" << std::endl;
		
		content << std::string((std::istreambuf_iterator<char>(content_end)), std::istreambuf_iterator<char>());
		this->setBody(content.str());
		return true;
	}

	return false;
}


////////////////////
// ERRORS
////////////////////
void		Response::initResponseMessageMap()
{
	this->_responseMessages[200] = "OK";					// OKKKKK
	this->_responseMessages[201] = "CREATED";				// Created
	this->_responseMessages[202] = "ACCEPTED";				// Accepted
	this->_responseMessages[204] = "NO_CONTENT";			// No content
	this->_responseMessages[403] = "FORBIDDEN";				// you dont have rights to access file
	this->_responseMessages[404] = "FILE_NOT_FOUND";		// target doesnt exist
	this->_responseMessages[405] = "METHOD_NOT_ALLOWED";	// method not supported
	this->_responseMessages[413] = "PAYLOAD_TOO_LARGE";		// client_max_bodysize < requestbody
	this->_responseMessages[500] = "INTERNAL_ERROR";		// smthg had gone wrong internaly, mostly part of cgi

}

void		Response::checkErrors()
{
	std::string errorMessage = strerror(errno);

	if (errno != 0 && !this->_isSetToError) // if _isSetToErro is dont want to print other errno
	{
		Logger::Write(Logger::DEBUG, RED, "strerror(errno) : " + errorMessage);
		if (errorMessage == "Permission denied")
			this->setToErrorPage(403);
		if (errorMessage == "No such file or directory")
			this->setToErrorPage(404);

		errno = 0;
	}
}

void		Response::setToErrorPage(int errorNumber)
{
	std::ifstream error_page;
	std::string errorNbrString = intToStr(errorNumber);

	this->setResponseCode(errorNumber);
	this->setContentType("text/html");
	if(!this->_location.getErrorPage()[errorNbrString].empty())
	{
		error_page.open(this->_location.getErrorPage()[errorNbrString]);
		std::string str((std::istreambuf_iterator<char>(error_page)), std::istreambuf_iterator<char>());
		this->setBody(str);
	}
	else
		this->setBody(this->generateDefaultErrorPage(errorNbrString, this->_responseMessages[errorNumber]));

	this->_isSetToError = true;
}

std::string		Response::generateDefaultErrorPage(std::string errorNbr, std::string message)
{
	std::ifstream content_1("files/default_pages/default_error_1.html");
	std::ifstream content_2("files/default_pages/default_error_2.html");
	std::ifstream content_3("files/default_pages/default_error_3.html");
	std::ostringstream body;

	body << std::string((std::istreambuf_iterator<char>(content_1)), std::istreambuf_iterator<char>());
	body << "error " << errorNbr;
	body << std::string((std::istreambuf_iterator<char>(content_2)), std::istreambuf_iterator<char>());
	body << "<h1>" << errorNbr << "</h1>";
	body << "<p>" << message << "</p>";
	body << std::string((std::istreambuf_iterator<char>(content_3)), std::istreambuf_iterator<char>());

	return (body.str());
}

////////////////////
// UTILS
////////////////////

bool	Response::isValidMethod(std::string key)
{
	
	std::vector<std::string> acceptedMethods = this->_location.getAcceptedMethod();

	// check if method is include in http 1.1
	if (!isValidHttpMethod(key))
		return (false);

	// if empty = accept all methods
	if (acceptedMethods.empty())
		return (true);

	for (std::vector<std::string>::iterator it = acceptedMethods.begin(); it != acceptedMethods.end(); ++it)
		if (key == *it)
			return (true);

	return (false);
}

bool	Response::isValidHttpMethod(std::string key)
{
	std::string listOfvalidHttpMethods[8] = {"GET", "HEAD", "POST", "PUT", "DELETE", "OPTIONS", "TRACE"}; // see https://tools.ietf.org/html/rfc7231 - RFC 7231
	std::vector<std::string> validHttpMethods;
	validHttpMethods.assign(listOfvalidHttpMethods, listOfvalidHttpMethods + 8);

	for (std::vector<std::string>::iterator it = validHttpMethods.begin(); it != validHttpMethods.end(); ++it)
		if (key == *it)
			return (true);

	return (false);
}

std::string	Response::getIndexTarget()
{
	std::vector<std::string> vIndex = this->_location.getIndex();

	if (vIndex.empty())
		return(this->_req->getTarget());

	for(std::vector<std::string>::iterator it = vIndex.begin(); it != vIndex.end(); ++it)
	{

		std::string target(safeUrlJoin(this->_req->getAbsoluteTargetPath(), *it));
		std::ifstream 	f(target.c_str());
		if (f.good())
		{
			f.close();
			return (safeUrlJoin(this->_req->getTarget(), *it));
		}
		f.close();
	}
	// std::ifstream indexTry;

	return (this->_req->getTarget());
}

bool		Response::isIndexPagePresent()
{
	std::vector<std::string> vIndex = this->_location.getIndex();
	if (vIndex.empty())
		return(false);

	for(std::vector<std::string>::iterator it = vIndex.begin(); it != vIndex.end(); ++it)
	{
		std::string target(safeUrlJoin(this->_req->getAbsoluteTargetPath(), *it));

		std::ifstream 	f(target.c_str());
		if (f.good())
		{
			f.close();
			return (true);
		}
		f.close();
	}
	return (false);
}

bool		Response::isDirectory()
{
	DIR *directory;

	if ((directory = opendir(this->_req->getAbsoluteTargetPath().c_str())))
		return true;
	return false;
}

std::string	Response::getContentType(std::string target)
{
	//kudos to the guy who wrote those variables
	std::string extension[67] = {"php", "aac", "abw", "arc", "avi", "azw", "bin", "bz", "bz2", "csh", "css", "csv", "doc", "docsx", "eot", "epub", "gif", "htm", "html", "ico",
	"ics", "jar", "jpeg", "jpg", "js", "json", "mid", "midi", "mpeg", "mpkg", "odp", "ods", "odt", "oga", "ogv", "ogx", "otf", "png", "pdf", "ppt", "pptx", "rar", "rtf", "sh"
	"svg", "swf", "tar", "tif", "tiff", "ts", "ttf", "vsd", "wav", "weba", "webm", "webp", "woff" ,"woff2", "xhtml", "xls", "xlsx","xml", "xul", "zip", "3gp", "3g2", "7z"};
	std::string content_type[67] = {"text/html", "audio/aac", "application/x-abiword", "application/octet-stream", "video/x-msvideo", "application/vnd.amazon.ebook", "application/octet-stream", "application/x-bzip",
	"application/x-bzip2", "application/x-csh", "text/css", "text/csv", "application/msword", "application/vnd.openxmlformats-officedocument.wordprocessingml.document", "application/vnd.ms-fontobject",
	"application/epub+zip", "image/gif", "text/html", "text/html", "image/x-icon", "text/calendar", "application/java-archive", "image/jpeg", "image/jpeg" ,"application/javascript", "application/json",
	"audio/midi","audio/midi", "video/mpeg", "	application/vnd.apple.installer+xml", "application/vnd.oasis.opendocument.presentation", "application/vnd.oasis.opendocument.spreadsheet", "application/vnd.oasis.opendocument.text", "audio/ogg", "video/ogg", "application/ogg", "font/otf", "image/png", "application/pdf", "application/vnd.ms-powerpoint", "application/vnd.openxmlformats-officedocument.presentationml.presentation",
	"application/x-rar-compressed", "application/rtf", "application/x-sh", "image/svg+xml", "application/x-shockwave-flash", "application/x-tar", "image/tiff", "image/tiff", "application/typescript", "font/ttf", "application/vnd.visio", "audio/x-wav", "audio/webm", "video/webm", "image/webp", "font/woff", "font/woff2", "application/xhtml+xml", "application/vnd.ms-excel", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet",
	"application/xml", "application/vnd.mozilla.xul+xml", "application/zip", "video/3gpp", "video/3gpp2", "application/x-7z-compressed"};

	int i = target.size() - 1;
	while (i >= 0 && target[i] != '.')
		i--;

	std::string ext = target.substr(i + 1, target.size() - 1);

	int j = 0;
	while (j < 67)
	{
		if (extension[j] == ext)
			return (content_type[j]);
		j++;
	}

	return ("text/plain");
}

void Response::logResponse()
{
	Logger::Write(Logger::DEBUG, BLU, "response : header\n\n" + this->getHeader() + "\n-------\n");
	Logger::Write(Logger::MORE, BLU, "response : body\n\n" + this->getBody() + "\n-------\n");
}

////////////////////
// SET RESPONSE / BODY / CONTENT TYPE (protect rewrite for errors)
////////////////////
void	Response::setResponseCode(int responseCode)
{
	if (!this->_isSetToError)
		this->_responseCode = responseCode;
}

void	Response::setBody(std::string body)
{
	if (!this->_isSetToError)
		this->_body = body;
}

void	Response::setContentType(std::string contentType)
{
	if (!this->_isSetToError)
		this->_contentType = contentType;
}

////////////////////
// GETTERS
////////////////////
std::string Response::getHeader()
{
	return (this->_header);
}

std::string Response::getBody()
{
	return (this->_body);
}

std::string Response::getResponse()
{
	return (this->_response);
}

std::string		Response::getResponseCodeStr()
{
	std::ostringstream oss;
	oss << this->_responseCode;
	return (oss.str());
}

int				Response::getResponseCode()
{
	return(this->_responseCode);
}

std::string		Response::getResponseCodeMessage()
{
	return (this->_responseMessages[this->_responseCode]);
}

std::string		Response::getContentLength()
{
	std::ostringstream oss;
	oss << this->_body.size();
	return (oss.str());
}


std::ostream &	operator<<(std::ostream & o, Response & rhs)
{
	(void)rhs;
	o << "In this response we have :\n";
	o << "Index path : "  << "\n";
	// o << "Response : " << rhs.getResponse() << "\n";

	return ( o );
}
