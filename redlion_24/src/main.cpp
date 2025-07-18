#include "../includes/Webserv.hpp"
#include "../includes/Http_server.hpp"
#include "../includes/Server_Conf_Parser.hpp"


//Tasks to do 
//build a simple one listener server
//add multiple host servers
//handle all the errors mangement bad connctions , socket c function failing etc...
//check the hangup of the client using nc

int main(int ac, char *av[])
{
    (void)ac;
    try{
        Http_server core(av[1]);
        if(core.check_init_http_server()){

            std::cerr << "Error while parsing the config file" << std::endl;
            return(1);
        }
        core.init_server_blocks();
        core.socket_main_loop();
    }
    catch(int num)
    {
        if(num == 1)
            std::cerr << "Webserv: file for config not found" << std::endl;
        else if(num == 2)
            std::cerr << "Webserv: epoll ctl fails when adding the fds" << std::endl;
        else if(num == 3)
            std::cerr << "Webserv: file for config not found" << std::endl;

    }
    catch (std::exception &e){
        std::cout << "exception starts" << std::endl;
    }
    catch(...){
        std::cout << "Error while parsing the config file" << std::endl;
    }
    //start the event loop
    
}
