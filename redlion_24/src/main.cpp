#include "../includes/Webserv.hpp"
#include "../includes/Http_server.hpp"
#include "../includes/Server_Conf_Parser.hpp"


//Tasks to do 
//build a simple one listener server
//add multiple host servers
//handle all the errors mangement bad connctions , socket c function failing etc...


int main(int ac, char *av[])
{
    //parse config file
    (void)ac;

    (void)av;
    // std::vector<ConfigNode> master;
    // size_t index;
    // index = 0;
    // Server_Conf_Parser ps(av[1]);
    // ps.read_data();
    // ps.parse_data(master, index);
    // exit(2);

    //TODO
    //init the server blocks
    Http_server core;
    core.init_server_blocks();

    core.socket_main_loop();
    //start the event loop
    
}
