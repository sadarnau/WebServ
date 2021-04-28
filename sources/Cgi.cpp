#include "Cgi.hpp"

// Cgi::Cgi(Request *req, Location *loc)
Cgi::Cgi(Request *req)
{
	this->_req = req;
	this->_initEnv();
	this->_envC = this->_envToCArray();
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

	this->_env["PATH_INFO"] =  this->_req->getTarget() + this->_req->getQueryString();
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
	this->_env["REQUEST_URI"] = this->_req->getUrlTargetPath() + "?" + this->_req->getQueryString();

	this->_env["SCRIPT_NAME"] = this->_req->getUrlTargetPath() + this->_req->getSelectedLocation().getCgi();
	// Le chemin virtuel vers le script étant exécuté. Exemple : « /cgi-bin/script.cgi »

	this->_env["SERVER_NAME"] = ip;
	this->_env["SERVER_PORT"] = port;
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SERVER_SOFTWARE"] = "webserv";
}

std::string		processCgi()
{
	// https://n-pn.fr/t/2318-c--programmation-systeme-execve-fork-et-pipe
	std::string ret;

	return (ret);
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
