#include "../includes/Server_block.hpp"


//todo add a method for pushing another server_name when parsing the config file


// int Server_block::get_port() const {}
struct sockaddr_in *Server_block::get_ip_addr(){
	return (&server_ip);
}


//method for getting all server names
std::vector<std::string> Server_block::get_Snames() const{
	return(server_names);
}


//default constructor add local host and port 80 as basic server block
Server_block::Server_block(){
	server_names.push_back("localhost");
	server_names.push_back("local");
	server_ip.sin_family = AF_INET;
	server_ip.sin_addr.s_addr = INADDR_ANY;
	server_ip.sin_port = htons(8080);
	index_flag = false;

}

Server_block::Server_block(std::string name_server, int port){
	(void)port;
	(void)name_server;
	server_names.push_back("barca");
	server_names.push_back("barcelona");
	server_ip.sin_family = AF_INET;
	server_ip.sin_addr.s_addr = INADDR_ANY;
	server_ip.sin_port = htons(4221);
	index_flag = false;

}
void Server_block::set_port(std::string port){
	//TODO add a check for the port if its valid
	int p = atoi(port.c_str());
	if(p > 0 && p < 65536)
		server_ip.sin_port = htons(p);
	else
		std::cout << "Invalid port number, using default port 8080" << std::endl;
}
void  Server_block::set_sname(std::vector <std::string> &vect){
	for(size_t i = 0; i < vect.size(); i++){
		server_names.push_back(vect[i]);
	}
}
void  Server_block::set_dir_listen(bool a){
	(void)a;
	//TODO add a check for the index flag
	index_flag = a;
}
void  Server_block::set_err_pages(std::vector <std::string> &list){
	for(size_t i = 0; i < list.size(); i++){
		error_pages.push_back(list[i]);
	}
}
void Server_block::set_ip_addr(std::string ip){
	//TODO add a check for the ip if its valid
	if(inet_pton(AF_INET, ip.c_str(), &server_ip.sin_addr) <= 0){
		std::cout << "Invalid IP address, using default IP address
}

//paramertized constructor for init ip and port and server name in one go
// Server_block::Server_block(std::string ip, std::string port,std::vector<std::string> server_names, bool index){}


//deconstructor for freeing any dynamic allocation
Server_block::~Server_block(){}
//copy constructors could be useful in the future
Server_block::Server_block(const Server_block &obj){
	(void)obj;
}
Server_block & Server_block::operator=(const Server_block &obj){
	(void)obj;
	return(*this);
}