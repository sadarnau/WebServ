#include "Webserv.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Logger.hpp"

int main(int ac, char *av[])
{
	Webserv		webserv;

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
		// close(webserv.getInSocket());
	}
	close(webserv.getFd());

	return 0;
}