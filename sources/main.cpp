#include "Webserv.hpp"
#include "Cluster.hpp"

static int serverFd;

void		handle_signal(int sig_num)
{
	Logger::Write(Logger::ERROR, std::string(RED), "You've just killed the server, an advice : CHANGE THE PORT TO MAKE IT QUICK\n\n", true);
	if (sig_num == SIGINT)
		close(serverFd);
}

int main(int ac, char *av[])
{
	Cluster cluster;

	Logger::Start(Logger::DEBUG);

	if (ac > 2)
	{
		Logger::Write(Logger::ERROR, std::string(RED), "Only wo args plllllz\n\n", true);
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

	cluster.lanchServices();

	// std::vector<int> list = cluster.getFdList();
	// for (std::vector<int>::iterator it = list.begin() ; it != list.end() ; it++)
	// 	close( *it );

	// close(webserv.getFd());

	return 0;

}