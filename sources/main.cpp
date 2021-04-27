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
	Logger::Write(Logger::INFO, std::string(GRN), "See you\n", true);
	if (sig_num == SIGINT)
		close(serverFd);
}

int main(int ac, char *av[])
{
	Cluster cluster;

	Logger::Start(Logger::DEBUG);

	if (ac > 3)
	{
		Logger::Write(Logger::ERROR, std::string(RED), "Only one arg allowed\n\n", true);
		return 1;
	}
	else if (ac == 2)
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