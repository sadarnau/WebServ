#include "Webserv.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include <sys/select.h>
#include <list>

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
	{
		if (webserv.initialization(av[1]))
			return (1);
	}
	else
		if (webserv.initialization("files/default.conf"))
			return (1);

	fd_set	copyMasterSet = webserv.getMasterSet();

	while(1)
	{
		// We have to make a copy of the master fd set because select() will change bits
		// of living fds  (( man 2 select ))
		copyMasterSet = webserv.getMasterSet();

		Logger::Write(Logger::INFO, std::string(GRN), "I'm waiting for a request...\n\n", true);

		int	nbSocket = select(webserv.getMaxFd() + 1, &copyMasterSet, 0, 0, 0); //to do : check if we can write in fd (writefds)

		if (nbSocket < 0)
		{
			Logger::Write(Logger::ERROR, std::string(RED), "Select has messed up everything...\n", true);
			return (1);
		}

		if (FD_ISSET(webserv.getFd(), &copyMasterSet)) // if serv fd changed -> new connection
		{
			Logger::Write(Logger::INFO, std::string(GRN), "New connection !\n\n", true);
			webserv.acceptConexion();
		}

		// We go throught every open fds to see if we have something new to read
		std::vector<int> list = webserv.getFdList();
		for (std::vector<int>::iterator it = list.begin() ; it != list.end() ; it++)
		{
			if (FD_ISSET(*it, &copyMasterSet))
				webserv.handleRequest( *it );		//if so we send a response without checking if we can write...
		}
	}

	std::vector<int> list = webserv.getFdList();
	for (std::vector<int>::iterator it = list.begin() ; it != list.end() ; it++)
		close( *it );

	close(webserv.getFd());

	return 0;
}