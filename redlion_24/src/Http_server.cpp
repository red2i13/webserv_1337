#include "../includes/Http_server.hpp"


//new todo
//DONEadd a timer function for checking timeout 
//DONEadd a function that a read full request until pushes them into the connection
//DONEadd a function that send the response in order
//DONE change epoll from level to edge triggered
//DONEcheck the header for closed connection
// STILL DOING You're popping responses without checking if the send was successful or complete.

int Http_server::make_socket_nonblocking(int fd){
    //throw an error internal server error
    int flags = fcntl(fd, F_GETFL ,0);
    if(flags < 0)
        return(-1);
    flags |= O_NONBLOCK;
    int t = fcntl(fd, F_SETFL, flags);
    if(t < 0)
        return(-1);
    return(0);
}

int Http_server::init_server_blocks(){
    int i = 0;
    std::vector<Server_block*>::iterator it;
    for(it = blocks.begin() ; it != blocks.end(); it++){
        struct sockaddr *addr = reinterpret_cast<struct sockaddr *>((*it)->get_ip_addr());
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if(fd < 0){
            return(1);
        }
        make_socket_nonblocking(fd);
        socket_fds.push_back(fd);
        int opt = 5;
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
 const char* Http_server::ParsingFails::what() const throw(){
    return("Error in parsing of the config file\n");
}
// int Http_server::get_block_id(int fd){
//     int block_index = -1;
//     for(size_t i = 0; i < socket_fds.size(); i++) {
//         if(socket_fds[i] == fd) {

//             block_index = i;
//             break;
//         }
//     }   
//     return(block_index);
// }
int Http_server::can_parse_complete_request(const std::string &buffer){
    if((buffer.find("\r\n\r\n") != std::string::npos) || (buffer.find("\n\n") != std::string::npos)) {
        return(1);
    }
    return(0);
}
// int process_request(HttpRequest &req){
//     HttpResponse res;

    
// }

int Http_server::handle_client_io(int it_fd){
    //part to change
    //************parse the request here med part***************
    Connection &conn = connections[events[it_fd].data.fd];
    char buffer[2048];
    ssize_t bytes;
    if(events[it_fd].events & EPOLLIN){        
        while((bytes = recv(events[it_fd].data.fd, buffer, sizeof(buffer), 0)) > 0)
        {
            std::cout << "***************************************************************************" << std::endl;
            // if(bytes == -1)
            //     return(1);
            if(!bytes)
            {
                std::cerr << "Error reading from client fd: " << events[it_fd].data.fd << std::endl;
                close(events[it_fd].data.fd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, events[it_fd].data.fd, &ev);
                connections.erase(events[it_fd].data.fd);
                //todo : remove connction from struct
                return(0);
            }
            conn.buffer.append(buffer, bytes);
            std::cout << "hello" << std::endl;
        }
        std::cout << "len of the buffer" << strlen(buffer) << std::endl;
        while(can_parse_complete_request(conn.buffer)){
            //find the first end of the request then append it to the conn.request
            size_t end_request;
            if ((end_request = conn.buffer.find("0\r\n\r\n")) != std::string::npos){
                end_request+=5;
            }
            else if ((end_request = conn.buffer.find("\r\n\r\n")) != std::string::npos)
                end_request += 4;
            HttpRequest req;
            if(!req.parse(conn.buffer.substr(0, end_request))){
                req.bad_req = true;
            }
            conn.requests.push(req);
            conn.buffer.erase(0, end_request + 4); // Remove the parsed request from
        }
        conn.mode = READING;
        //
        conn.last_activity = time(0);
}
    if(!conn.requests.empty() && conn.mode == READING){
        //if the request is parsed and ready to be processed
        //change the mode to processing
        conn.mode = PROCESSING;
        HttpResponse res;
        size_t index = fd_block_map[events[it_fd].data.fd];
        if (!conn.requests.front().is_keep_alive)
            conn.mode = CLOSED; 
        if ((conn.requests.size() > 0) && (conn.requests.front().cgi_flag))
        {
            handle_cgi(conn.requests.front(), res);
            if (res.status_code == 500)
            {
                res.set_header("Content-Type", "text/html");
                res.set_body("<html><body><h1>500 Internal Server Error</h1><p>Cannot process CGI request.</p></body></html>");
            }
            conn.responses.push(res);
            conn.requests.pop();
        }
        else{

            handle_request(conn.requests.front(), res, *blocks[index]);
            conn.responses.push(res);
            conn.requests.pop();
        }

    }
    if(events[it_fd].events & EPOLLOUT && !conn.responses.empty()){
        
        HttpResponse res = conn.responses.front();

        std::string response_str = res.to_string();
        send(events[it_fd].data.fd, response_str.c_str(), response_str.length(), 0);
        if(conn.mode == CLOSED) 
        {
            std::cout << "i am closed\n";
            close(events[it_fd].data.fd);
            epoll_ctl(epfd, EPOLL_CTL_DEL, events[it_fd].data.fd, &ev);
            connections.erase(events[it_fd].data.fd);
            return(0);
        }
        conn.last_activity = time(0);
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
 
    epfd = epoll_create(1);
    if(epfd < 0){
        perror("Error epoll: ");
        return(1);
    }
    //add fd of every listening server to the epoll
    for(size_t it_block = 0 ; it_block < blocks.size(); it_block++){
        ev.data.fd = socket_fds[it_block];
        ev.events = EPOLLIN;
        if(epoll_ctl(epfd, EPOLL_CTL_ADD, socket_fds[it_block], &ev) == -1)
            throw 2;
        block_num[socket_fds[it_block]] = it_block;

    }
    for(;;){
        int ready_fd = epoll_wait(epfd, events, MAX_EVENT, 1000);
        if(ready_fd < 0){
            check_connection_timeout();
            continue;
        }
        //check timeout for clients
        std::cout << "num of ready fds " << ready_fd  << std::endl;
        for(int it_fd = 0; it_fd < ready_fd; it_fd++)
        {
            std::cout << events[it_fd].data.fd << " event number" << std::endl;
            if(checkIfListen(events[it_fd].data.fd))
            {
                c_fd = accept(events[it_fd].data.fd, reinterpret_cast<struct sockaddr *>(&c_addr), reinterpret_cast<socklen_t*>(&len_c_addr));      
                std::cout << "setting fd return " << make_socket_nonblocking(c_fd) << std::endl;
                fd_block_map[c_fd] = block_num[events[it_fd].data.fd];
                connections[c_fd] = Connection(c_fd, blocks[block_num[events[it_fd].data.fd]], READING);
                if(c_fd != -1)
                std::cout << "Client connected succesfuly c_fd value: " << c_fd <<  std::endl; 
                //adding client fd to epoll insantnce
                ev.data.fd = c_fd;
                ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, c_fd, &ev);
            }
            else if (events[it_fd].events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP))
            {
                std::cout << "&%^&$*^&%^#&*^&%$^^&" << std::endl;
                close(events[it_fd].data.fd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, events[it_fd].data.fd, &ev);
                connections.erase(events[it_fd].data.fd);
            }
            else
            {
                if(handle_client_io(it_fd) == 1)
                    continue;
            }
            check_connection_timeout();
        }
    }
    
    return(0);
}

void Http_server::check_connection_timeout(){
    time_t current_time = time(0);
    for ( std::map<int, Connection>::iterator it = connections.begin() ;connections.size() > 0 &&  it != connections.end() ;){
        // std::cout << "diff " << current_time - it->second.last_activity << "size map "<< connections.size() << std::endl;
        if(current_time - it->second.last_activity > 5) {
            std::cout << "Connection " << it->first << " timed out." << std::endl;
            close(it->first);
            epoll_ctl(epfd, EPOLL_CTL_DEL, it->first, &ev);
            connections.erase(it);
        }
        else
            ++it;
    }
}
Http_server::Http_server(){
    Server_block *def = new Server_block();
    Server_block *def1 = new Server_block("test", 80);
    blocks.push_back(def);
    blocks.push_back(def1);
}

Connection::Connection(int n_fd, Server_block *ptr, cnx_mode m)  : fd(n_fd),is_processing(false), last_activity(time(0)) , buffer(), requests(), responses(), mode(m), server(ptr) {

}


Http_server::Http_server(char *ConfigFile){
    if(!ConfigFile)
        throw 1;
    size_t index;

    //open a file error.log and specify the date of the error and whats the problem
    index = 0;
    Server_Conf_Parser ps(ConfigFile);

    if(ps.read_data())
        // throw(4);
        throw ParsingFails();
    ps.parse_data(master, index);
  
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
    blocks.clear();
}
