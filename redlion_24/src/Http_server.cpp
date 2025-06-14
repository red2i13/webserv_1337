#include "../includes/Http_server.hpp"


//todo
//Add multiplexer io model to the while loop
//protect the all the call that return a file descriptor when failing
//the backlog argument of listen how much shoudl i allow for pending
//find a solution for previlieged port below 1024 exit or handle it
//*Done search about setsockopt and why its useful ?
//*Done using cast of c or c++

int Http_server::init_server_blocks(){
    int i = 0;
    std::vector<Server_block*>::iterator it;
    for(it = blocks.begin() ; it != blocks.end(); it++){
        struct sockaddr *addr = reinterpret_cast<struct sockaddr *>((*it)->get_ip_addr());
        socket_fds.push_back(socket(AF_INET, SOCK_STREAM, 0));
        int opt = 1;
        setsockopt(socket_fds[i], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        if(bind(socket_fds[i], addr, sizeof(struct sockaddr_in)) == -1{
            perror("bind fails: ");
            return(1);
        }
        listen(socket_fds[i], 1);
        std::cout << "initialization num " << i << std::endl;
        i++;
    }
    return(0);
}
int Http_server::checkIfListen(int fd){
    for(size_t i = 0; i < socket_fds.size() ; i++){
        if(socket_fds[i] == fd)
            return(1);
    }
    return(0);
}

int Http_server::socket_main_loop(){
    int c_fd;
    struct sockaddr_in c_addr;
    int len_c_addr = sizeof(c_addr);
    std::vector<Server_block*>::iterator it;
    //adding epoll
    //note tune the max event to optimize the server when there is to much traffic
    struct epoll_event ev;
    int MAX_EVENT = 64;
    struct epoll_event arr[MAX_EVENT];
    int epfd = epoll_create(1);
    if(epfd < 0){
        perror("Error epoll: ");
        return(1);
    }
    //add fd of every listening server to the epoll
    for(size_t it_block = 0 ; it_block < blocks.size(); it_block++){
        ev.data.fd = socket_fds[it_block];
        ev.events = EPOLLIN;
        epoll_ctl(epfd, EPOLL_CTL_ADD, socket_fds[it_block], &ev);

    }
    for(;;){
        int ready_fd = epoll_wait(epfd, arr, MAX_EVENT, -1);
        std::cout << "num of ready fds " << ready_fd << std::endl;
        for(int it_fd = 0; it_fd < ready_fd; it_fd++)
        {
            if(checkIfListen(arr[it_fd].data.fd))
            {
                c_fd = accept(arr[it_fd].data.fd, reinterpret_cast<struct sockaddr *>(&c_addr), reinterpret_cast<socklen_t*>(&len_c_addr));
                if(c_fd != -1)
                std::cout << "Client connected succesfuly c_fd value: " << c_fd <<  std::endl; 
                //adding client fd to epoll insantnce
                ev.data.fd = c_fd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, c_fd, &ev);
            }
            else
            {
                //************parse the request here med part***************
                char buffer[1024];
                ssize_t bytes;
                while ((bytes = recv(arr[it_fd].data.fd, buffer, sizeof(buffer), 0)) > 0) {
                    request.append(buffer, bytes);
                    if (request.find("\r\n\r\n") != std::string::npos)
                    break;
                }
                if (!req.parse(request))
                {
                    std::cerr<<"Failed to parse !\n";
                    return 1;
                }
                
                std::cout << "Received request:\n" << request << std::endl;
                //delete the file descriptor form epoll instance
                write(arr[it_fd].data.fd, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 23\r\n\r\nThis website is working" , 89);
                close(arr[it_fd].data.fd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, arr[it_fd].data.fd, &ev);
            }
        }
        //****************************************************** */
        //write the response (before check with epoll if the opration is possible)
    }
    return(0);
}
 
Http_server::Http_server(){
    Server_block *def = new Server_block();
    Server_block *def1 = new Server_block("test", 80);
    blocks.push_back(def);
    blocks.push_back(def1);
}
Http_server::~Http_server(){

}
