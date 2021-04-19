#include "Response.hpp"

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

void	Response::buildBody()
{

}

void	Response::processGet()
{
	std::string		root;
	std::string		target;
	
	std::string		content;

	root = "files";							// Change to config root
	if (this->_req->getTarget() == "/")
		target = root + "/index.html";     // Change to default index in conf
	else
		target = root + this->_req->getTarget();

	// Check if the file can be open and if so put it in content

	std::ifstream 	f(target.c_str()); // open index.html
	if (f.good())
	{
		this->_responseCode = 200;
		this->_responseCodeMessage = "OK";
		this->_contentType = "text/html";
		std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>()); //initialize str with index.html content
		this->_body = str;
	}
	else
	{
		this->_responseCode = 400;
		this->_responseCodeMessage = "";
		this->_contentType = "text/html";
		std::ifstream error_page("files/default_error_pages/404.html");
		std::string str((std::istreambuf_iterator<char>(error_page)), std::istreambuf_iterator<char>());
		this->_body = str;
		std::cout << "Error opening the index file...\n";
	}

	f.close();
	
	return ;
}

void	Response::processPost()
{

}

void	Response::send()
{
	write(this->_socket, this->_response.c_str() , this->_response.length());	//to protect
}


std::string Response::getResponse()
{
	return (this->_response);
}
