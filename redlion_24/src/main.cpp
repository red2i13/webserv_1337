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
    Server_Conf_Parser ps(av[1]);
    ps.read_data();
    ps.clean_data();
    exit(2);

    //TODO
    //init the server blocks
    Http_server core;
    core.init_server_blocks();

    core.socket_main_loop();
    //start the event loop
    
}
