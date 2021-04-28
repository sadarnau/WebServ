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
	if (ac == 3)
	{
		std::string str(av[2]);
		if (!str.compare("more"))
		{
			Logger::Start(Logger::MORE);
			Logger::Write(Logger::MORE, GRN, "Logger in MORE mod");
		}
		else if (!str.compare("debug"))
		{
			Logger::Start(Logger::DEBUG);
			Logger::Write(Logger::DEBUG, GRN, "Logger in DEBUG mod");
		}
		else if (!str.compare("info"))
		{
			Logger::Start(Logger::INFO);
			Logger::Write(Logger::INFO, GRN, "Logger in INFO mod");
		}
		else if (!str.compare("error"))
		{
			Logger::Start(Logger::ERROR);
			Logger::Write(Logger::ERROR, GRN, "Logger in ERROR mod");
		}
		else if (!str.compare("none"))
		{
			Logger::Start(Logger::NONE);
			Logger::Write(Logger::NONE, GRN, "Logger in NONE mod");
		}

	}
	else
	{
		Logger::Start(Logger::DEBUG);
		Logger::Write(Logger::DEBUG, GRN, "Logger in DEBUG mod");
	}

	if (ac > 3)
	{
		Logger::Write(Logger::ERROR, std::string(RED), "Only two/three args allowed\n\n");
		return 1;
	}
	else if (ac == 2 || ac == 3)
	{
		if (cluster.initialization(av[1]))
			return (1);
	}
	else
		if (cluster.initialization("files/default.conf"))
			return (1);

	if (cluster.lanchServices())
		return (1); // exception ??

	// std::vector<int> list = cluster.getFdList();
	// for (std::vector<int>::iterator it = list.begin() ; it != list.end() ; it++)
	// 	close( *it );

	// close(webserv.getFd());

	return 0;

}