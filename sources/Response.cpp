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

}

void	Response::buildHeader()
{

}

void	Response::buildBody()
{

}

void	Response::processGet()
{
	std::ifstream 	f("files/index.html"); // open index.html
	std::string		content;

	// Check if the file can be open and if so put it in content

	if (f.good())
	{
		std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>()); //initialize str with index.html content
		content = str;
	}
	else
		std::cout << "Error opening the index file...\n";

	f.close();


	std::ostringstream oss;
	oss << "HTTP/1.1 200 OK\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << content.size() << "\r\n";	oss << "\r\n";
	oss << content;

	this->_response = oss.str();
	
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
