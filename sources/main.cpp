#include "Webserv.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Logger.hpp"

int main()
{
	Webserv		webserv;
	Response	response;
	Config		config;

	// config.parseFile("test.conf");
	// return 0;

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