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
        if(bind(socket_fds[i], addr, sizeof(struct sockaddr_in)) == -1){
            perror("bind fails: ");
            return(1);
        }
        if(listen(socket_fds[i], 64) == -1){
            return(perror("listen fails: "), 1);
        }
        
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
    std::map<int, int> fd_block_map;
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
                int block_index = -1;
                for(size_t i = 0; i < socket_fds.size(); i++) {
                    if(socket_fds[i] == arr[it_fd].data.fd) {
                        block_index = i;
                        break;
                    }
                }                
                fd_block_map[c_fd] = block_index;
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
                char buffer[2048];
                ssize_t bytes;
                while ((bytes = recv(arr[it_fd].data.fd, buffer, sizeof(buffer), 0)) > 0) {
                    request.append(buffer, bytes);
                    if (request.find("\r\n\r\n") != std::string::npos)
                    {
                        break;
                    }
                }
                std::cout << "==== RAW REQUEST ====\n" << request << "\n=====================" << std::endl;

                if (!req.parse(request)) {
                    std::cerr << "Failed to parse!\n";
                    close(arr[it_fd].data.fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, arr[it_fd].data.fd, &ev);
                    continue;
                }
                if (req.target == "/cgi-bin")
                {
                    req.cgi_flag = true;
                    //function cgi dial adnan
                }

                //delete the file descriptor form epoll instance
                // write(arr[it_fd].data.fd, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 23\r\n\r\nThis website is working" , 89);
                handle_request(req, res, *blocks[fd_block_map[arr[it_fd].data.fd]]);
                std::string response_str = res.to_string();
                send(arr[it_fd].data.fd, response_str.c_str(), response_str.length(), 0);
                close(arr[it_fd].data.fd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, arr[it_fd].data.fd, &ev);
                request.clear();
                res = HttpResponse();
                req = HttpRequest();
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


Http_server::Http_server(char *ConfigFile){
    size_t index;

    index = 0;
    Server_Conf_Parser ps(ConfigFile);
    try{
        ps.read_data();
        ps.parse_data(master, index);
    }
    catch(...){
        std::cout << "Error while parsing the config file" << std::endl;
    }
}


int Http_server::check_init_http_server(){
    std::vector<ConfigNode> *ptr;
    std::vector<ConfigNode> *n_dir;
    

    for(size_t i = 0; i < master.size() ; i++){
        if(master[i].name == "http"){
            ptr = &master[i].children;
            for(size_t j = 0; j < ptr->size() ; j++){
                if((*ptr)[j].name == "server")
                {
                    Server_block *new_svb = new Server_block();
                    n_dir = &(*ptr)[j].children;
                    for(size_t k = 0; k < (*n_dir).size(); k++){
                        if((*n_dir)[k].name == "server_name")
                            new_svb->set_sname((*n_dir)[k].values);
                        else  if((*n_dir)[k].name == "error_page")
                            new_svb->set_err_pages((*n_dir)[k].values);
                        else  if((*n_dir)[k].name == "listen")
                            new_svb->set_ip_host((*n_dir)[k].values);
                        else  if((*n_dir)[k].name == "location")
                        {
                            std::cout  << "test location "<< ((*n_dir)[k].values)[0] << std::endl;
                            new_svb->set_location((*n_dir)[k].values[0], (*n_dir)[k].children[0].values);
                        }
                        else if((*n_dir)[k].name == "autoindex")
                            new_svb->set_dir_listen((*n_dir)[k].values[0] == "on");
                        else if((*n_dir)[k].name == "post_dir")
                            new_svb->set_upload_path((*n_dir)[k].values);
                    }
                    blocks.push_back(new_svb);
                }
            }
        }
    }    
    return(0);
}

Http_server::~Http_server(){

}
