#include "Response.hpp"

////////////////////
// Coplien's
////////////////////
Response::Response(Request *req, long socket)
{
	this->initResponseMessageMap();

	this->_isSetToError = false;
	this->_location = req->getSelectedLocation();
	this->_req = req;
	this->_socket = socket;
	this->_httpVersion = "HTTP/1.1";


	this->buildResponse();
}

Response::Response(Response const & src)
{
	*this = src;

	return ;
}

Response::~Response(void)
{
	return ;
}

Response & Response::operator=(Response const & rhs)
{
	(void)rhs;

	return (*this);
}


////////////////////
// METHODS
////////////////////

void	Response::buildResponse(void)
{
	std::string	requestMethod = this->_req->getMethod();
	
	errno = 0;

	// METHODS
	if (this->_req->getBadRequest())
		this->setToErrorPage(400);
	else if (this->_req->getHttpVersion() != "HTTP/1.1")
		this->setToErrorPage(505);
	else if (!this->isValidAuthorization())
		this->setToErrorPage(401);
	else if (!this->isValidMethod())
		this->setToErrorPage(405);
	else if (this->_req->getContentLength() > this->_location.getClientMaxBodySize())
		this->setToErrorPage(413);
	else
	{
		if (requestMethod == "GET" || requestMethod == "POST" || requestMethod == "HEAD")
			this->processGetPostHead();
		else if (requestMethod == "PUT")
			this->processPut();
		else if (requestMethod == "TRACE")
			this->processTrace();
		else if (requestMethod == "OPTIONS")
			this->processOptions();
		else if (requestMethod == "DELETE")
			this->processDelete();
	}
	// BUILD HEADER AND RESPONSE
	this->buildHeader();

	if (requestMethod == "HEAD")
		this->_response = this->_header;
	else
		this->_response = this->_header + this->_body;

	return ;
}

void	Response::buildHeader(void)
{
	std::ostringstream header;

	header << this->_httpVersion << " " << this->_responseCode << " " << this->_responseMessages[this->_responseCode] << "\r\n";

	this->_headers["Content-Type"] = this->_contentType;
	this->_headers["Content-Length"] = Utils::longToStr(this->_body.size());
	this->_headers["Date"] = Utils::getDate();
	this->_headers["Host"] = this->_req->getSelectedLocation().getListen();
	if (!this->_req->getSelectedLocation().getServerName().empty())
		this->_headers["Server"] = this->_req->getSelectedLocation().getServerName();
	else
		this->_headers["Server"] = std::string("Webserv");
	if (this->_responseCode == 401)
		this->_headers["WWW-Authenticate"] = "Basic realm=\"acces to webserv\"";
	if (this->_responseCode == 201)
		this->_headers["Location"] = this->_req->getUrlTargetPath();

	std::map<std::string, std::string> tmpHeaders = this->_headers;
	std::map<std::string, std::string> tmpCgiHeaders = this->_cgiheaders;

	for (std::map<std::string, std::string>::const_iterator it = tmpCgiHeaders.begin(); it != tmpCgiHeaders.end(); it++)
		if (it->first != "Status")
			tmpHeaders[it->first] = it->second;

	for (std::map<std::string, std::string>::const_iterator it = tmpHeaders.begin(); it != tmpHeaders.end(); it++)
		if (!it->second.empty())
			header << it->first << ": " << it->second << "\r\n";

	this->_header = header.str() + "\r\n";

	return ;
}


////////////////////
// HTTP METHODS
////////////////////
void	Response::processGetPostHead(void)
{
	// Directory Request
	if (this->isDirectory())
	{
		if (this->isIndexPagePresent())
			this->_req->updateTarget(this->getIndexTarget());
		else if(this->_location.getAutoindex() == "on" && this->autoIndexResponse())  //autoIndexResponse return (true) on success
				return ;
	}
	this->checkErrors();


	// CGI
	if (!this->_location.getCgiPath().empty() && (this->_location.getCgiExt() == Utils::getExtension(this->_req->getTarget())))
	{

		Cgi		cgi(this->_req);

		if(cgi.processCgi())
		{
			this->setBody(cgi.getResult());
			this->_cgiheaders = cgi.getCgiHeaders();
			this->setResponseCode(200);
		}
		else
			this->setToErrorPage(500);
	}
	else
	{

		this->setResponseCode(200);
		this->setBody(Utils::getFileContent(this->_req->getAbsoluteTargetPath().c_str()));
		this->checkErrors();
	}
	this->setContentType(this->getContentType(this->_req->getTarget()));

	return ;
}

void	Response::processPut(void)
{
	std::string 	toWrite(this->_req->getBody());
	std::string		path(this->_req->getAbsoluteTargetPath());
	std::ofstream	file;

	if (Utils::isPathAFile(path))
	{
		file.open(path.c_str());
		if (!file.is_open())
			this->checkErrors();
		else
		{
			file << toWrite;
			file.close();
			this->setResponseCode(200);
		}
	}
	else
	{
		file.open(path.c_str(), std::ofstream::out | std::ofstream::trunc);
		if (!file.is_open())
			this->checkErrors();
		else
		{
			file << toWrite;
			file.close();
			this->setResponseCode(201);
		}
	}

	return ;
}

void	Response::processOptions(void)
{
	std::string 				allow;
	std::vector<std::string>	acceptedMethods = this->_location.getAcceptedMethod();

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

	return ;
}

void	Response::processTrace(void)
{
	this->setResponseCode(200);
	this->setContentType("message/html");
}

void	Response::processDelete(void)
{
	std::ifstream	f(this->_req->getAbsoluteTargetPath().c_str());

	if (f.good())
	{
		if (std::remove(this->_req->getAbsoluteTargetPath().c_str()) == 0)
			this->setResponseCode(204);
		else
			this->setResponseCode(202);
	}
	else
		this->checkErrors();

	return ;
}


////////////////////
// AUTO INDEX
////////////////////
bool		Response::autoIndexResponse(void)
{
	DIR				*directory;
	struct dirent	*dircontent;

	if ((directory = opendir(this->_req->getAbsoluteTargetPath().c_str())))
	{
		this->setResponseCode(200);
		this->setContentType("text/html");

		std::string content_start = Utils::getFileContent("files/default_pages/auto_index_start.html");
		std::ostringstream content;
		std::string content_end = Utils::getFileContent("files/default_pages/auto_index_end.html");

		content << content_start;
		content << "<h1>Directory : " << this->_req->getUrlTargetPath() << "</h1>";
		content << "<ul>" << std::endl;

		while((dircontent = readdir(directory)))
		{
			content << "<li>" << "<a href='" << Utils::safeUrlJoin(this->_req->getUrlTargetPath(), dircontent->d_name) <<
			"'>" << dircontent->d_name << "</a>" << "</li>" << std::endl;
		}
		content << "</ul>" << std::endl;
		
		content << content_end;
		this->setBody(content.str());
		closedir(directory);
		return (true);
	}

	return (false);
}


////////////////////
// MESSAGES / ERRORS
////////////////////
void		Response::initResponseMessageMap(void)
{
	this->_responseMessages[200] = "OK";							// OKKKKK
	this->_responseMessages[201] = "CREATED";						// Created
	this->_responseMessages[202] = "ACCEPTED";						// Accepted
	this->_responseMessages[204] = "NO_CONTENT";					// No content
	this->_responseMessages[400] = "BAD_REQUEST";					// Request parsing find an error in request
	this->_responseMessages[401] = "UNAUTHORIZED";					// Unauthorized
	this->_responseMessages[403] = "FORBIDDEN";						// you dont have rights to access file
	this->_responseMessages[404] = "FILE_NOT_FOUND";				// target doesnt exist
	this->_responseMessages[405] = "METHOD_NOT_ALLOWED";			// method not supported
	this->_responseMessages[413] = "PAYLOAD_TOO_LARGE";				// client_max_bodysize < requestbody
	this->_responseMessages[500] = "INTERNAL_ERROR";				// smthg had gone wrong internaly, mostly part of cgi
	this->_responseMessages[505] = "HTTP_VERSION_NOT_SUPPORTED";	// httpVersion != HTTP/1.1


	return ;
}

void		Response::checkErrors(void)
{
	std::string errorMessage = strerror(errno);

	if (errorMessage == "Not a directory")
		return ;

	if (errno != 0 && !this->_isSetToError) // if _isSetToError is true we dont want to print other errno
	{
		Logger::Write(Logger::DEBUG, RED, "strerror(errno) : " + Utils::intToStr(errno) + " " + errorMessage);
		if (errorMessage == "Permission denied")
			this->setToErrorPage(403);
		if (errorMessage == "No such file or directory")
			this->setToErrorPage(404);
		errno = 0;
	}

	return ;
}

void		Response::setToErrorPage(int errorNumber)
{
	std::string		errorNbrString = Utils::intToStr(errorNumber);

	this->setResponseCode(errorNumber);
	this->setContentType("text/html");
	if(!this->_location.getErrorPage()[errorNbrString].empty())
		this->setBody(Utils::getFileContent(this->_location.getErrorPage()[errorNbrString]));
	else
		this->setBody(this->generateDefaultErrorPage(errorNbrString, this->_responseMessages[errorNumber]));

	this->_isSetToError = true;

	return ;
}

std::string		Response::generateDefaultErrorPage(std::string errorNbr, std::string message)
{
	std::string		content_1 = Utils::getFileContent("files/default_pages/default_error_1.html");
	std::string		content_2 = Utils::getFileContent("files/default_pages/default_error_2.html");
	std::string		content_3 = Utils::getFileContent("files/default_pages/default_error_3.html");
	std::ostringstream	body;

	body << content_1;
	body << "error " << errorNbr;
	body << content_2;
	body << "<h1>" << errorNbr << "</h1>";
	body << "<p>" << message << "</p>";
	body << content_3;

	return (body.str());
}

////////////////////
// UTILS
////////////////////

bool	Response::isValidAuthorization(void)
{
	std::string					authentication(this->_location.getAuthentication());
	std::string					authorization(this->_req->getHeaders()["Authorization"]);
	std::vector<std::string>	split;

	if (authentication.empty())
		return (true);
	if (authorization.empty())
		return (false);
	Utils::splitStringToVector(authorization, split);
	if (split[0].compare("Basic"))
		return (false);
	authorization = Utils::decode64(split[1]);
	if (authentication.compare(authorization))
		return (false);
	Logger::Write(Logger::INFO, GRN, "Authenticaton Sucessfull !");
	return (true);
}

bool	Response::isValidMethod(void)
{
	std::vector<std::string>	acceptedMethods = this->_location.getAcceptedMethod();
	std::ostringstream			oss;

	// check if method is include in http 1.1
	if (!isValidHttpMethod())
		return (false);

	// if empty = accept all methods
	if (acceptedMethods.empty())
		return (true);
	
	for (std::vector<std::string>::iterator it = acceptedMethods.begin(); it != acceptedMethods.end(); ++it)
		if (this->_req->getMethod() == *it)
			return (true);

	return (false);
}

bool	Response::isValidHttpMethod(void)
{
	std::string listOfvalidHttpMethods[7] = {"GET", "HEAD", "POST", "PUT", "DELETE", "OPTIONS", "TRACE"}; // see https://tools.ietf.org/html/rfc7231 - RFC 7231

	for (int i = 0; i < 7; i++)
	{
		if (listOfvalidHttpMethods[i] == this->_req->getMethod())
			return (true);
	}
	return (false);
}

std::string	Response::getIndexTarget(void) // commentaire a supprimer ?
{
	std::vector<std::string> vIndex = this->_location.getIndex();

	if (vIndex.empty())
		return(this->_req->getTarget());

	for(std::vector<std::string>::iterator it = vIndex.begin(); it != vIndex.end(); ++it)
	{

		std::string target(Utils::safeUrlJoin(this->_req->getAbsoluteTargetPath(), *it));
		std::ifstream 	f(target.c_str());
		if (f.good())
		{
			f.close();
			return (Utils::safeUrlJoin(this->_req->getTarget(), *it));
		}
		f.close();
	}
	// std::ifstream indexTry;				//ici

	return (this->_req->getTarget());
}

bool		Response::isIndexPagePresent(void)
{
	std::vector<std::string> vIndex = this->_location.getIndex();

	for(std::vector<std::string>::iterator it = vIndex.begin(); it != vIndex.end(); ++it)
	{
		if (*it == "")
			return (false);
		std::string target(Utils::safeUrlJoin(this->_req->getAbsoluteTargetPath(), *it));

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

bool		Response::isDirectory(void)
{
	DIR *directory;

	if ((directory = opendir(this->_req->getAbsoluteTargetPath().c_str())))
	{
		closedir(directory);
		return (true);
	}

	return (false);
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

////////////////////
// SET RESPONSE / BODY / CONTENT TYPE (protect rewrite for errors)
////////////////////
void	Response::setResponseCode(int responseCode)
{
	if (!this->_isSetToError)
		this->_responseCode = responseCode;

	return ;
}

void	Response::setBody(std::string body)
{
	if (!this->_isSetToError)
		this->_body = body;

	return ;
}

void	Response::setContentType(std::string contentType)
{
	if (!this->_isSetToError)
		this->_contentType = contentType;

	return ;
}

////////////////////
// GETTERS
////////////////////
std::string Response::getHeader(void)
{
	return (this->_header);
}

std::string Response::getBody(void)
{
	return (this->_body);
}

std::string Response::getResponse(void)
{
	return (this->_response);
}

std::string		Response::getResponseCodeStr(void)
{
	std::ostringstream oss;
	oss << this->_responseCode;

	return (oss.str());
}

int				Response::getResponseCode(void)
{
	return(this->_responseCode);
}

std::string		Response::getResponseCodeMessage(void)
{
	return (this->_responseMessages[this->_responseCode]);
}

long			Response::getHeaderLength(void)
{
	return (this->_header.size());
}

long			Response::getBodyLength(void)
{
	return (this->_body.size());
}

long			Response::getResponseLength(void)
{
	return (this->_response.size());
}

////////////////////
// LOG
////////////////////
void Response::logResponse(int serverNbr)
{
	Logger::Write(Logger::INFO, BLU, "server[" + Utils::intToStr(serverNbr) + "] : response sent [code: " +
		this->getResponseCodeStr() + "] [message: " + this->getResponseCodeMessage() 
		+ "] [length: " + Utils::intToStr(this->getHeaderLength()) + " + " + Utils::intToStr(this->getBodyLength()) + 
		+ " = " + Utils::intToStr(this->getResponseLength()) +"]");
	Logger::Write(Logger::DEBUG, WHT, " response : header\n" + this->getHeader());
	Logger::Write(Logger::MORE, BLU, " full response :\n" + this->getResponse());

	return ;
}