/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadarnau <sadarnau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/11 13:05:37 by sadarnau          #+#    #+#             */
/*   Updated: 2021/04/11 16:49:01 by sadarnau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// includes du sujet
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <algorithm>
#include <exception>
#include <ctime>
#include <cstring>

#include <sys/types.h>      //socket function
#include <sys/socket.h>     //Define (AF_INET..)
#include <netinet/in.h>     //sockaddr struct
#include <unistd.h>   	  	//read function
#include <sys/time.h>		//gettimeofday()

#include <sstream>			//test
#include <fstream>			//test

/*

		Functions to fill the header

std::string	setStatus( std::string res )
{
	res = "HTTP/1.1 200 OK\n";

	return (res);
}

std::string	setDate( std::string res )
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);

	res += "Date: Sun, 11 Apr 2021 15:13:34 GMT\n";

	return (res);
}

std::string	setContentType( std::string res )
{
	res += "Content-Type: text/html\n";

	return (res);
}

std::string	setContentLength( std::string res )
{
	res += "Content-Length: 155\n";
	res += "Content-Location: /index.html\n";
	return (res);
}

std::string	setLastModified( std::string res )
{
	res += "Last-Modified: Sun, 11 Apr 2021 15:14:34 GMT\n";

	return (res);
}

std::string	setServer( std::string res )
{
	res += "Server: Test/0.0 (test)\n\n";

	return (res);
}

std::string addContent( std::string res, std::string content )
{
	res += content;

	return (res);
}
*/


std::string	response()
{
	std::ifstream f("index.html"); // open index.html
	std::string content;

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

	std::string	res = oss.str();

	//printing the header on the terminal
	std::cout << "\n---------\n" << "Response:\n\n" << res << "\n\n-------\n\n";

	return (res);
}

int main()
{
	int fd_serv;
	struct sockaddr_in address;

	fd_serv = socket(AF_INET, SOCK_STREAM, 0); // to pprotect

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; //htonl ??
	address.sin_port = htons(8080);

	memset(address.sin_zero, 0, sizeof(address.sin_zero));  // to pprotect

	bind(fd_serv, (struct sockaddr *)&address, sizeof(address));  // to pprotect

	listen(fd_serv, 5); // 5 = number of max connections

	int in_socket;
	unsigned int addrlen = sizeof(address);

	while(1)
	{
		std::cout << "I'm waiting for something to come up...\n\n";
		in_socket = accept(fd_serv, (struct sockaddr *)&address, (socklen_t*)&addrlen);  // to pprotect

		char buff[10000];
		read( in_socket , buff, 10000);  // to protect
		std::cout << "Request :\n\n" << buff << std::endl;

		std::string	res = response();

		write(in_socket, res.c_str() , res.length());	//to protect
		std::cout << "Message delivered...\n\n";
		close(in_socket);
	}
	close(fd_serv);
	
	return 0;
}