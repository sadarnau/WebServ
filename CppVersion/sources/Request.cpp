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

void	Request::printRequest( void )
{
	std::cout << "\n----------\nRequest :\n\n" << this->buff << "-----------\n\n";

	return ;
}

int		Request::getInSock( void )
{
	return (this->in_socket);
}
