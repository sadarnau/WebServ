/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadarnau <sadarnau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/11 16:25:31 by sadarnau          #+#    #+#             */
/*   Updated: 2021/04/11 17:26:22 by sadarnau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <iostream>
# include <sstream>
# include <fstream>


class Response
{
private:

	std::string	response;

public:

	Response( void );								//default constructor
	// Response( std::string name );					//constructor
	Response( Response const & src);  				//copy
	~Response( void );								//destructor
	Response & operator=( Response const & rhs );	//overload operator =

	void		fillResponse( void );
	std::string	getResponse( void );
};

#endif
