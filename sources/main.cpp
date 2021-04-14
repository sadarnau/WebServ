/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/11 16:57:03 by sadarnau          #+#    #+#             */
/*   Updated: 2021/04/14 14:43:09 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Logger.hpp"

int main()
{
	Webserv		webserv;
	Response	response;

	webserv.initialization();
	Logger::Start(Logger::DEBUG);
	while(1)
	{
		Logger::Write(Logger::INFO, std::string(GRN), "I'm waiting for something to come up...\n\n", true);

		webserv.handleRequest();

		response.fillResponse();
		//printing the header on the terminal
		Logger::Write(Logger::DEBUG, std::string(BLU), "\n---------\nResponse:\n\n" + response.getResponse() + "\n-------\n\n", true);

		write(webserv.getInSocket(), response.getResponse().c_str() , response.getResponse().length());	//to protect
		Logger::Write(Logger::INFO, std::string(GRN), "Message delivered...\n\n", true);
		close(webserv.getInSocket());
	}
	close(webserv.getFd());

	return 0;
}