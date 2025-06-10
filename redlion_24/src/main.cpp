#include "../includes/Webserv.hpp"
#include "../includes/Http_server.hpp"



//Tasks to do 
//build a simple one listener server
//add multiple host servers
//handle all the errors mangement bad connctions , socket c function failing etc...


int main()
{
    //parse config file
    //TODO
    //init the server blocks
    Http_server core;
    core.init_server_blocks();

    core.socket_main_loop();
    //start the event loop
    
}
