/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadarnau <sadarnau@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/11 16:57:03 by sadarnau          #+#    #+#             */
/*   Updated: 2021/04/11 17:26:48 by sadarnau         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webServ.hpp"
#include "Response.hpp"

int main()
{
    webServ		webserv;
    Response	response;

    webserv.initialization();

	while(1)
	{
		std::cout << "I'm waiting for something to come up...\n\n";

        webserv.handeRequest();

		response.fillResponse();

		//printing the header on the terminal
		std::cout << "\n---------\n" << "Response:\n\n" << response.getResponse() << "\n\n-------\n\n";

		write(webserv.getInSocket(), response.getResponse().c_str() , response.getResponse().length());	//to protect
		std::cout << "Message delivered...\n\n";
		close(webserv.getInSocket());
	}
	close(webserv.getFd());
	
	return 0;
}