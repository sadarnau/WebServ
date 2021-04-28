#include "Webserv.hpp"
#include "Response.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Cluster.hpp"
#include <sys/select.h>
#include <list>
#include <csignal>

static int serverFd;

void		handle_signal(int sig_num)
{
	Logger::Write(Logger::INFO, GRN, "See you\n");
	if (sig_num == SIGINT)
		close(serverFd);
}

int main(int ac, char *av[])
{
	Cluster cluster;

	Logger::Start(Logger::DEBUG);

	if (ac > 3)
	{
		Logger::Write(Logger::ERROR, RED, "usage : ./webserv (-d for debug) (path to .conf file)\n");
		return 1;
	}
	else if (ac == 3) // check '-d'
	{
		std::string str(av[1]);
		if (!str.compare("-d"))
		{
			if (cluster.initialization(av[2], 1))
				return (1);
		}
		else
		{
			Logger::Write(Logger::ERROR, RED, "usage : ./webserv (-d for debug) (path to .conf file)\n");
			return (1);
		}
	}
	else if (ac == 2)
	{
		std::string str(av[1]);
		if (!str.compare("-d"))
		{
			if (cluster.initialization("files/default.conf", 1))
				return (1);
		}
		else
			if (cluster.initialization(av[1], 0))
				return (1);
	}
	else
		if (cluster.initialization("files/default.conf", 0))
			return (1);

	if (cluster.lanchServices())
		return (1); // exception ??

	// std::vector<int> list = cluster.getFdList();
	// for (std::vector<int>::iterator it = list.begin() ; it != list.end() ; it++)
	// 	close( *it );

	// close(webserv.getFd());

	return 0;

}