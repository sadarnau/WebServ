/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadarnau <sadarnau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/11 16:25:25 by sadarnau          #+#    #+#             */
/*   Updated: 2021/04/12 16:22:15 by sadarnau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request( void )
{
	return ;
}

Request::Request( int in_sock, char *buff ) : in_socket(in_sock)
{
	std::string	tmp(buff);	//convert char* to std::string
	this->buff = tmp;
	this->parseRequest(this->buff);
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
    this->buff = rhs.buff;
	this->in_socket = rhs.in_socket;

	return ( *this );
}

bool	Request::isValidHeader(std::string header)
{
	std::string list_of_accepted_headers[18] = {"Accept-Charsets", "Accept-Language", "Allow", "Authorization", "Content-Language",
												"Content-Length", "Content-Location", "Content-Type", "Date", "Host", "Last-Modified",
												"Location", "Referer", "Retry-After", "Server", "Transfer-Encoding", "User-Agent",
												"WWW-Authenticate"};
	std::vector<std::string> accepted_headers;
	accepted_headers.assign(list_of_accepted_headers, list_of_accepted_headers + 18);

	for (std::vector<std::string>::iterator it = accepted_headers.begin(); it != accepted_headers.end(); ++it)
		if (header == *it)
			return (true);

	return (false);
}

bool	Request::isRequestMethod(std::string key)
{
	std::string list_of_accepted_methods[8] = {"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"}; // see https://tools.ietf.org/html/rfc7231 - RFC 7231
	std::vector<std::string> accepted_methods;
	accepted_methods.assign(list_of_accepted_methods, list_of_accepted_methods + 8);

	for (std::vector<std::string>::iterator it = accepted_methods.begin(); it != accepted_methods.end(); ++it)
		if (key == *it)
			return (true);

	return (false);
}

void	Request::parseRequest(std::string req)
{
	std::string				line;
    std::string				value;
    std::string				key;
	std::istringstream		stream_req(req);

	//apparently getline process last line 2 times (should be corrected with use of GNL)
	while (std::getline(stream_req, line))
    {
		std::stringstream	ss(line);
		ss >> key >> value;										// set the variables

		if (this->isRequestMethod(key))							// handle method line
		{
			this->method = key;
			this->target = value;
		}
		else													// handle anything else (= headers)
		{
			key = key.substr(0, key.length() - 1);				// delete char ':' at the end of key
			if (this->isValidHeader(key))
				this->headers[key] = value;
			else
				this->skipped_headers.push_back(key);
		}
	}
}

void	Request::printRequest( void )
{
	std::cout << YEL; // set color
	std::cout << "\n----------\nREQUEST OBJECT :\n\n" ;
	std::cout << std::setw(20) << "request->method" << " : " << this->method << std::endl;
	std::cout << std::setw(20) << "request->target " << " : " << this->target << std::endl << std::endl;

	std::cout << "Content of request->headers :" << std::endl << std::endl; 
	std::cout << std::setw(20) << "KEY" << " : " << "VALUE" << std::endl << std::endl;
	for (std::map<std::string, std::string>::const_iterator it = this->headers.begin(); it != this->headers.end(); ++it)
	{
		std::cout << std::setw(20) << it->first << " : " << it->second << std::endl;
	}

	//SKIPPED HEADERS
	std::cout << std::endl << std::endl << "Skipped headers : ";
	for (std::vector<std::string>::iterator it = this->skipped_headers.begin(); it != this->skipped_headers.end(); ++it)
		std::cout << " " << *it;
	std::cout << std::endl << std::endl;

	// RAW REQUEST
	std::cout << "Raw request :" << std::endl << std::endl;
	std::cout << this->buff;
	std::cout << "----------\n\n";
	std::cout << RESET;

	return ;
}

int		Request::getInSock( void )
{
	return (this->in_socket);
}
