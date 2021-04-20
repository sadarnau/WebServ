#include "Response.hpp"

////////////////////
// Coplien's
////////////////////
Response::Response(Config *conf, Request *req, int socket)
{
	this->_conf = conf;
	this->_req = req;
	this->_socket = socket;

	_httpVersion = "HTTP/1.1";

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

	if (requestMethod == "GET")
		this->processGet();
	else if (requestMethod == "POST")
		this->processPost();

	this->buildHeader();
	this->_response = this->_header + this->_body;
}

void	Response::buildHeader()
{
	// Minimal header accepted :
	// HTTP/1.1 200 OK
	// Content-Type: text/html
	// Content-Length: 0

	std::ostringstream header;
	header << this->_httpVersion << " " << this->_responseCode << " " << this->_responseCodeMessage << "\n";
	header << "Content-Type: " << this->_contentType << "\n";
	header << "Content-Length: " << this->_body.size(); 

	header << "\n\n";						//End of header
	this->_header = header.str();
}


////////////////////
// GET
////////////////////
void	Response::processGet()
{

	std::string		content;

	std::string		root = "files/www";			// Change to config->location->root
	std::string		auto_index= "on";			// Change to config->location->auto_index
	std::string 	index_page = "index.html";	// Change to index in conf 

	std::string 	target;
	// Directory Request
	if (this->isDirectory())
	{
		if(!isIndexPagePresent() && auto_index == "on" && this->autoIndexRequest())  //autoIndexRequest return true on success
			return ;
		else if (isIndexPagePresent())
			target = this->getIndexPath();
	}
	else
		target = this->_req->getRelativeTargetPath();

	
	//TO_DO : Here comes the block where you check the file ext and define content_type

	// Check if the file can be open and cerate response
	std::ifstream 	f(target.c_str()); // open file
	if (f.good())
	{
		this->setHeaders(200, "OK", "text/html");
		std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>()); //initialize str with index.html content
		this->_body = str;
	}
	else
	{
		this->setHeaders(404, "FILE_NOT_FOUND", "text/html");
		std::ifstream error_page("files/default_pages/custom_404.html");
		std::string str((std::istreambuf_iterator<char>(error_page)), std::istreambuf_iterator<char>());
		this->_body = str;
	}

	f.close();
}


////////////////////
// POST
////////////////////
void	Response::processPost()
{

}


////////////////////
// AUTO INDEX
////////////////////
bool		Response::autoIndexRequest()
{
	std::string		auto_index = "on";						// Change to config->location->auto_index

	DIR *directory;
	struct dirent *dircontent;

	if ((directory = opendir(this->_req->getRelativeTargetPath().c_str())))
	{
		setHeaders(200, "OK", "text/html");

		std::ifstream content_start("files/default_pages/auto_index_start.html");
		std::ostringstream content;
		std::ifstream content_end("files/default_pages/auto_index_end.html");

		content << std::string((std::istreambuf_iterator<char>(content_start)), std::istreambuf_iterator<char>());

		content << "<h1>Directory : " << this->_req->getTarget() << "</h1>";
		content << "<ul>" << std::endl;

		while((dircontent = readdir(directory)))
		{
				content << "<li>" << "<a href='" << this->_req->getTarget();
				if (this->_req->getTarget().back() != '/')			// this ensure folder path to have '/' when traget is not root
					content << '/';
				content << dircontent->d_name <<"'>";
				content << dircontent->d_name << "</a>" << "</li>" << std::endl;
		}
		content << "</ul>" << std::endl;
		
		content << std::string((std::istreambuf_iterator<char>(content_end)), std::istreambuf_iterator<char>());
		this->_body = content.str();
		return true;
	}

	return false;
}


////////////////////
// UTILS
////////////////////
std::string	Response::getIndexPath()
{
	std::string 	index_page = "index.html";	// Change to index in conf 

	if (this->_req->getRelativeTargetPath().back() != '/')
		return(this->_req->getRelativeTargetPath() + "/" + index_page); 
	else
		return (this->_req->getRelativeTargetPath() + index_page);
}

bool		Response::isIndexPagePresent()
{
	std::string 	index_page = "index.html";	// Change to index in conf 

	std::string		target;

	if (index_page.empty())						//If no index in conf file
		return false;

	target = getIndexPath();

	std::ifstream 	f(target.c_str());

	return (f.good());
}

bool		Response::isDirectory()
{
	DIR *directory;

	if ((directory = opendir(this->_req->getRelativeTargetPath().c_str())))
		return true;
	return false;
}


////////////////////
// GETTERS / SETTERS
////////////////////
void	Response::setHeaders(int responseCode, std::string responseCodeMessage, std::string contentType)
{
	this->_responseCode = responseCode;
	this->_responseCodeMessage = responseCodeMessage;
	this->_contentType = contentType;
}


std::string Response::getResponse()
{
	return (this->_response);
}
