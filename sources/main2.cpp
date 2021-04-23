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

	fd_set	copyMasterSet = cluster.getMasterSet();

	serverFd = cluster.getServerList()[0].getFd();

	// test :
	Webserv webserv = cluster.getServerList()[0];

	int test = 1;

	std::cout << cluster;
	std::cout << webserv;

	while(test)
	{
		signal(SIGINT, handle_signal);

		// We have to make a copy of the master fd set because select() will change bits
		// of living fds  (( man 2 select ))
		copyMasterSet = cluster.getMasterSet();

		Logger::Write(Logger::INFO, std::string(GRN), "I'm waiting for a request...\n\n", true);

		int	nbSocket = select(cluster.getMaxFd() + 1, &copyMasterSet, 0, 0, 0); //to do : check if we can write in fd (writefds)

		if (nbSocket < 0)
		{
			// Logger::Write(Logger::ERROR, std::string(RED), "Select has messed up everything...\n", true);
			return (1);
		}

		if (FD_ISSET(serverFd, &copyMasterSet)) // if serv fd changed -> new connection
		{
			Logger::Write(Logger::INFO, std::string(GRN), "New connection !\n\n", true);
			int sock = cluster.getServerList()[0].acceptConexion();
			cluster.addSocketToMaster(sock);
		}

		// We go throught every open fds to see if we have something new to read
		std::vector<int> list = cluster.getFdList();
		for (std::vector<int>::iterator it = list.begin() ; it != list.end() ; it++)
		{
			if (FD_ISSET(*it, &copyMasterSet))
				webserv.handleRequest( *it );		//if so we send a response without checking if we can write...
		}
	}

	// std::vector<int> list = cluster.getFdList();
	// for (std::vector<int>::iterator it = list.begin() ; it != list.end() ; it++)
	// 	close( *it );

	close(webserv.getFd());

	return 0;

}