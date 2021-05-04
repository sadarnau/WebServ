#include "Cgi.hpp"

// Cgi::Cgi(Request *req, Location *loc)
Cgi::Cgi(Request *req)
{
	this->_req = req;
	this->_initEnv();
	this->_envC = this->_envToCArray();
	this->_initArgC();
}

Cgi::Cgi( Cgi const & src )
{
	*this = src;
	return ;
}


Cgi::~Cgi()
{
	//delete _envCFormat;
	if (this->_envC)
	{
		for (int i = 0; this->_envC[i]; i++)
			delete[] this->_envC[i];
		delete this->_envC;
	}
}

Cgi & Cgi::operator=( Cgi const & rhs)
{
	(void)rhs;
	// this->??? = rhs.???;
	return ( *this );
}

void    Cgi::_initEnv()
{
	std::string listen = this->_req->getSelectedLocation().getListen();
	std::string port = listen.substr(listen.find(":") + 1 , listen.size());
	std::string ip = listen.substr(0, listen.find(":"));

	// https://fr.wikipedia.org/wiki/Variables_d%27environnement_CGI
	std::map<std::string, std::string>	reqHeaders = this->_req->getHeaders();
	this->_env["AUTH_TYPE"] = reqHeaders["Authorization"];
	this->_env["CONTENT_LENGTH"] = reqHeaders["Content-Length"];
	this->_env["CONTENT_TYPE"] = reqHeaders["Content-Type"];
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";

	this->_env["PATH_INFO"] =  this->_req->getTarget();
	// Le chemin supplémentaire du script tel que donné par le client. Par exemple, si le serveur héberge le script « /cgi-bin/monscript.cgi » 
	// et que le client demande l'url « http://serveur.org/cgi-bin/monscript.cgi/marecherche », alors PATH_INFO contiendra « marecherche ».

	this->_env["PATH_TRANSLATED"] = this->_req->getAbsoluteTargetPath();
	// Contient le chemin demandé par le client après que les conversions virtuel → physique ont été faites par le serveur.

	this->_env["QUERY_STRING"] = this->_req->getQueryString();

	this->_env["REMOTE_ADDR"] = "";
	// L'adresse IP du client.

	this->_env["REMOTE_IDENT"] = "";
	// Nom d'utilisateur (distant) du client faisant la requête. Le serveur doit supporter l'identification RFC 931. Cette variable devrait être utilisée à des fins de journaux seulement.

	this->_env["REMOTE_USER"] = "";
	// Le nom d'utilisateur du client, si le script est protégé et si le serveur supporte l'identification.

	this->_env["REQUEST_METHOD"] = this->_req->getMethod();
	this->_env["REQUEST_URI"] = this->_req->getUrlTargetPath();

	this->_env["SCRIPT_NAME"] = this->_req->getUrlTargetPath();
	// Le chemin virtuel vers le script étant exécuté. Exemple : « /cgi-bin/script.cgi »

	this->_env["SERVER_NAME"] = ip;
	this->_env["SERVER_PORT"] = port;
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SERVER_SOFTWARE"] = "webserv";
}

void		Cgi::_initArgC()
{
	this->_argC = new char*[3];

	this->_argC[0] = new char[this->_req->getSelectedLocation().getCgi().size() + 1];
	this->_argC[0] = std::strcpy(this->_argC[0], this->_req->getSelectedLocation().getCgi().c_str());

	this->_argC[1] = new char[this->_req->getAbsoluteTargetPath().size() + 1];
	this->_argC[1] = std::strcpy(this->_argC[1], this->_req->getAbsoluteTargetPath().c_str());

	this->_argC[2] = 0;

	for (int i = 0; this->_argC[i]; i++)
		printf("%s\n", this->_argC[i]);

}

bool		Cgi::processCgi(std::string body)
{
	this->logCgi();
	(void)body;
	// https://n-pn.fr/t/2318-c--programmation-systeme-execve-fork-et-pipe

	// https://www.oreilly.com/openbook/cgi/ch04_02.html
	//	If the protocol is GET, read the query string from QUERY_STRING and/or the extra path information from PATH_INFO.
	//	If the protocol is POST, determine the size of the request using CONTENT_LENGTH and read that amount of data from the standard input.

	std::string result;
	pid_t		pid;
	int		status;

	// save STDIN and STDOUT state 
	int			stdIn = dup(STDIN_FILENO);
	int			stdOut = dup(STDOUT_FILENO);

	//creating fd for child execution
	FILE	*fIn = tmpfile();
	FILE	*fOut = tmpfile();
	int		fdIn = fileno(fIn);
	int		fdOut = fileno(fOut);

	// writing content of body in fdIn
	write(fdIn, this->_req->getBody().c_str(), this->_req->getBody().size());
	//repositioning file offset to start of fdIn 
	lseek(fdIn, 0, SEEK_SET);

	if ((pid = fork()) == -1)
	{
			Logger::Write(Logger::ERROR, RED, "cgi : fork failed");			
			close(fdIn);
			close(fdOut);
			fclose(fIn);
			fclose(fOut);
			return false;
	}
	else if(pid == 0)
	{
		// child

		// redirect STDIN and STDOUT to tmp fds
		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);

		// execute cgi
		if(execve(this->_req->getSelectedLocation().getCgi().c_str(), this->_argC, this->_envC) == -1)
		{
						
			dup2(stdIn, STDIN_FILENO);
			dup2(stdOut, STDOUT_FILENO);
			close(fdIn);
			close(fdOut);
			fclose(fIn);
			fclose(fOut);
			Logger::Write(Logger::ERROR, RED, "cgi : execve failed : " + std::string(strerror(errno)));

			return false;
		}
	}
	else
	{
		// parent
		char	buffer[1024];
		int		ret;

		wait(&status);
		lseek(fdOut, 0, SEEK_SET);

		// read on fdOut to get output
		while ((ret = read(fdOut, buffer, 1023)) != 0)
		{
        	buffer[ret] = 0;
			result += buffer;
		}
	}

	// restore STDIN and STDOUT
	dup2(stdIn, STDIN_FILENO);
	dup2(stdOut, STDOUT_FILENO);

	//close fds
	close(fdIn);
	close(fdOut);
	fclose(fIn);
	fclose(fOut);

	this->_result = result;

	return true;
}


char	**Cgi::_envToCArray()
{
	char    **res = new char*[this->_env.size() + 1];
	int     i = 0;

	for (std::map<std::string, std::string>::const_iterator it = this->_env.begin(); it != this->_env.end(); ++it)
	{
		std::string     elem = it->first + "=" + it->second;
		res[i] = new char[elem.size() + 1];
		res[i] = std::strcpy(res[i], elem.c_str());
		i++;
	}
	res[i] = 0;

	return (res);
}

void								Cgi::logCgi()
{
	printMap(this->_env, "cgi::_env\n");
}

std::string	Cgi::getResult()
{
	return (this->_result);
}