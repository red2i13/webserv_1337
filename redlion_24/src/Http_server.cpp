#include "../includes/Http_server.hpp"


//todo
//protect the all the call that return a file descriptor when failing
//search about setsockopt and why its useful ?
//using cast of c or c++
//the backlog argument of listen how much shoudl i allow for pending
//find a solution for previlieged port below 1024 exit or handle it
int Http_server::init_server_blocks(){
    int i = 0;
    std::vector<Server_block*>::iterator it;
    for(it = blocks.begin() ; it != blocks.end(); it++){


        struct sockaddr *addr = reinterpret_cast<struct sockaddr *>((*it)->get_ip_addr());
        socket_fds.push_back(socket(AF_INET, SOCK_STREAM, 0));
        int opt = 1;
        setsockopt(socket_fds[0], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if(bind(socket_fds[0], addr, sizeof(struct sockaddr_in)) == -1)
        {
            perror("bind fails: ");
            return(1);
        }
        listen(socket_fds[0], 1);
        std::cout << "initialization num " << i++ << std::endl;
    }
    return(0);
}

int Http_server::socket_main_loop(){
    int c_fd;
    struct sockaddr_in c_addr;
    int len_c_addr = sizeof(c_addr);

    std::vector<Server_block*>::iterator it;
    for(;;){
        for(it = blocks.begin() ; it != blocks.end(); it++){
            c_fd = accept(socket_fds[0], reinterpret_cast<struct sockaddr *>(&c_addr), reinterpret_cast<socklen_t*>(&len_c_addr));
            if(c_fd != -1)
            std::cout << "Client connected succesfuly c_fd value: " << c_fd <<  std::endl; 
        }
        close(c_fd);
    }
    return(0);
}
 
Http_server::Http_server(){
    Server_block *def = new Server_block();
    
    blocks.push_back(def);
}
Http_server::~Http_server(){

}
