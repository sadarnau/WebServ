/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/11 16:27:17 by sadarnau          #+#    #+#             */
/*   Updated: 2021/04/14 11:25:12 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "Request.hpp"
# include "Logger.hpp"
# include <unistd.h>		//read function	
# include <string>
# include <iostream>
# include <cwchar>
# include <cstring>
# include <netinet/in.h>     //sockaddr struct

class Webserv
{
private:

	int					fd;
	struct sockaddr_in	address;
	Request				inRequest;

public:

	Webserv( void );						 		//default constructor
	// Webserv( std::string name );					//constructor
	Webserv( Webserv const & src);  				//copy
	~Webserv( void );								//destructor
	Webserv & operator=( Webserv const & rhs );		//overload operator =

	void	initialization( void );
	void	fillAddress( void );
	void	handleRequest( void );
	int		getInSocket( void );
	int		getFd( void );
};

#endif
