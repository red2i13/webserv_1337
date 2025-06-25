#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "Webserv.hpp"
#include "Server_block.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
class Http_server{
    private:
        std::vector<int> socket_fds;
        std::vector<Server_block*> blocks;
        HttpRequest req;
        HttpResponse res;
        std::string request;
    public:
    Http_server();
    ~Http_server();
    // Http_server(const Http_server &obj);
    // Http_server & operator=(const Http_server &obj);
    int init_server_blocks();
    int socket_main_loop();
    int checkIfListen(int fd);
};

#endif