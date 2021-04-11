/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadarnau <sadarnau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/11 16:27:17 by sadarnau          #+#    #+#             */
/*   Updated: 2021/04/11 17:06:35 by sadarnau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "webServ.hpp"

webServ::webServ( void )
{
	return ;
}

// webServ::webServ( std::string name )
// {
// 	return ;
// }

webServ::webServ( webServ const & src )
{
	*this = src;
	return ;
}

webServ::~webServ( void )
{
	return ;
}

webServ & webServ::operator=( webServ const & rhs)
{
    // this->??? = rhs.???;
	(void)rhs;
	return ( *this );
}

void	webServ::initialization( void )
{
	this->fd = socket(AF_INET, SOCK_STREAM, 0); // to pprotect
	
	this->fillAddress();

	bind(this->fd, (struct sockaddr *)&this->address, sizeof(this->address));  // to pprotect

	listen(this->fd, 5); // 5 = number of max connections
	
	return ;
}

void	webServ::fillAddress( void )
{
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = INADDR_ANY; //htonl ??
	this->address.sin_port = htons(8080);

	memset(this->address.sin_zero, 0, sizeof(this->address.sin_zero));  // to pprotect

	return ;
}

void	webServ::handeRequest( void )
{
	int in_sock;

	unsigned int addrlen = sizeof(address);
	in_sock = accept(this->fd, (struct sockaddr *)&this->address, (socklen_t*)&addrlen);;
	
	char buff[10000];				// GNL maybe better ?
	read( in_sock , buff, 10000);	// to protect

	Request inRequest(in_sock, buff);
	
	inRequest.printRequest();
}

int		webServ::getInSocket( void )
{
	return (this->inRequest.getInSock());
}

int		webServ::getFd( void )
{
	return (this->fd);
}