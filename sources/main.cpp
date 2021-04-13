/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadarnau <sadarnau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/11 16:57:03 by sadarnau          #+#    #+#             */
/*   Updated: 2021/04/12 18:15:04 by sadarnau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Response.hpp"
#include "Config.hpp"

int main()
{
	Webserv		webserv;
	Response	response;

	webserv.initialization();
	
	while(1)
	{
		std::cout << "I'm waiting for something to come up...\n\n";

		webserv.handleRequest();

		response.fillResponse();

		//printing the header on the terminal
		std::cout << "\n---------\n" << "Response:\n\n" << response.getResponse() << "\n-------\n\n";

		write(webserv.getInSocket(), response.getResponse().c_str() , response.getResponse().length());	//to protect
		std::cout << "Message delivered...\n\n";
		close(webserv.getInSocket());
	}
	close(webserv.getFd());

	return 0;
}