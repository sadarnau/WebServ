/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadarnau <sadarnau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/11 16:25:26 by sadarnau          #+#    #+#             */
/*   Updated: 2021/04/11 17:03:39 by sadarnau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <iostream>

class Request
{
private:

	int			in_socket;
	std::string	buff;

public:

	Request( void );								//default constructor
	Request( int in_sock, char *buff );					//constructor
	Request( Request const & src);  				//copy
	~Request( void );								//destructor
	Request & operator=( Request const & rhs );		//overload operator =

	void	printRequest( void );
	int		getInSock( void );
};

#endif
