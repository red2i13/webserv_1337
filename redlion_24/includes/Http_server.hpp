#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "Webserv.hpp"

class Http_server{
    private:
        std::vector<std::string> server_names;
        std::vector<int> port;
    public:
    Http_server();
    ~Http_server();
    Http_server(const Http_server &obj);
    Http_server & operator=(const Http_server &obj);
    
    int socket_main_loop();
}

#endif