#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "Webserv.hpp"
#include "Server_block.hpp"
#include "HttpRequest.hpp"
#include "Server_Conf_Parser.hpp"
#include "HttpResponse.hpp"

class Http_server{
    private:
        //For ast parsing
        std::vector<ConfigNode> master;
        //Part for uprunning server
        std::vector<int> socket_fds;
        std::vector<Server_block*> blocks;
        HttpRequest req;
        HttpResponse res;
        std::string request;

    public:
        Http_server();
        Http_server(char *configFile);
        ~Http_server();
        // Http_server(const Http_server &obj);
        // Http_server & operator=(const Http_server &obj);
        int init_server_blocks();
        int check_init_http_server();
        int socket_main_loop();
        int checkIfListen(int fd);
        class ParsingFails : public std::exception{
            virtual const char* what() const throw();
	};
};

#endif