#include "Webserv.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Logger.hpp"

int main(int ac, char *av[])
{
	Webserv		webserv;
	Response	response;

	Logger::Start(Logger::DEBUG);

	if (ac > 2)
	{
		Logger::Write(Logger::ERROR, std::string(RED), "Only wo args plllllz\n\n", true);
		return 1;
	}
	else if (ac == 2)
		webserv.initialization(av[1]);
	else
		webserv.initialization("files/default.conf");

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