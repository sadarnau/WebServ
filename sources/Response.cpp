#include "Response.hpp"

Response::Response( void )
{
	return ;
}

// Response::Response( std::string name )
// {
// 	return ;
// }

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
    // this->??? = rhs.???;
	(void)rhs;
	return ( *this );
}

void	Response::fillResponse( void )
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

	this->response = oss.str();
	
	return ;
}

std::string	Response::getResponse( void )
{
	return(this->response);
}
