/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/11 16:27:17 by sadarnau          #+#    #+#             */
/*   Updated: 2021/04/13 11:17:34 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "Request.hpp"
# include <unistd.h>		//read function	
# include <string>
# include <iostream>
# include <cwchar>
# include <cstring>
# include <netinet/in.h>     //sockaddr struct

class webServ
{
private:

	int					fd;
	struct sockaddr_in	address;
	Request				inRequest;

public:

	webServ( void );						 		//default constructor
	// webServ( std::string name );					//constructor
	webServ( webServ const & src);  				//copy
	~webServ( void );								//destructor
	webServ & operator=( webServ const & rhs );		//overload operator =

	void	initialization( void );
	void	fillAddress( void );
	void	handleRequest( void );
	int		getInSocket( void );
	int		getFd( void );
};

#endif
