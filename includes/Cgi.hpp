#ifndef CGI_HPP
# define CGI_HPP

# include <string>

class Cgi
{
    private:
        std::string     auth_type;
        std::string     content_length;
        std::string     content_type;
        std::string     gateway_interface;
        std::string     path_info;
        std::string     path_translated;
        std::string     query_string;
        std::string     remote_add;
        std::string     remote_ident;
    public:
        Cgi();
};

#endif