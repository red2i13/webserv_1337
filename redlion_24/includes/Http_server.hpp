#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "Webserv.hpp"
#include "Server_block.hpp"
#include "HttpRequest.hpp"
#include "Server_Conf_Parser.hpp"
#include "HttpResponse.hpp"
#include "run_cgi.hpp"

#define MAX_EVENT 64

enum cnx_mode{
    READING,
    WRITING,
    PROCESSING
};
struct Connection {
    int fd;
    bool is_processing; // Indicates if the connection is currently processing a request
    time_t last_activity;
    std::string buffer;  // Accumulates received data
    std::queue<HttpRequest> requests;  // Parsed requests waiting to be processed
    std::queue<HttpResponse> responses; // Completed responses ready to be sent
    cnx_mode mode;
    Server_block *server;
    Connection(int n_fd, Server_block *ptr, cnx_mode m);
    Connection() : fd(-1), mode(READING), server(NULL) {} // Default constructor
};
class Http_server{
    private:
        //For ast parsing
        std::vector<ConfigNode> master;
        //Part for uprunning server
        std::vector<int> socket_fds;
        std::vector<Server_block*> blocks;
        std::map<int, Connection> connections;
        // HttpRequest req;
        // HttpResponse res;
        std::string request;
        std::map<int, int> fd_block_map;
        std::map<int , int> block_num;
        struct epoll_event ev;
        struct epoll_event events[MAX_EVENT];
        int epfd;
    public:
        Http_server();
        Http_server(char *configFile);
        ~Http_server();
        // Http_server(const Http_server &obj);
        // Http_server & operator=(const Http_server &obj);
        void check_connection_timeout();
        int can_parse_complete_request(const std::string &buffer);
        int init_server_blocks();
        int check_init_http_server();
        int socket_main_loop();
        int checkIfListen(int fd);
        int get_block_id(int fd);
        int handle_client_io(int fd);
        int make_socket_nonblocking(int fd);
        int process_request(HttpRequest &req, Connection &conn);
        class ParsingFails : public std::exception{
            virtual const char* what() const throw();
	};
};

#endif