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
    std::map<std::string, std::string>	reqHeaders = this->_req->getHeaders();
    this->_env["AUTH_TYPE"] = reqHeaders["Authorization"];
    this->_env["CONTENT_LENGTH"] = reqHeaders["Content-Length"];
    this->_env["CONTENT_TYPE"] = reqHeaders["Content-Type"];
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->_env["PATH_INFO"] = "";
    this->_env["PATH_TRANSLATED"] = "";
    this->_env["QUERY_STRING"] = this->_req->getQueryString();
    this->_env["REMOTE_ADDR"] = "";
    this->_env["REMOTE_IDENT"] = "";
    this->_env["REMOTE_USER"] = "";
    this->_env["REQUEST_METHOD"] = this->_req->getMethod();
    this->_env["REQUEST_URI"] = this->_req->getUrlTargetPath() + "?" + this->_req->getQueryString();
    this->_env["SCRIPT_NAME"] = "";
    this->_env["SERVER_NAME"] = "";
    this->_env["SERVER_PORT"] = "";
    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_env["SERVER_SOFTWARE"] = "webserv";
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
