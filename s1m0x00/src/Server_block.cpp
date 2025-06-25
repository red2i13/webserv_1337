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