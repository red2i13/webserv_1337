#include "../includes/Server_block.hpp"


//todo add a method for pushing another server_name when parsing the config file
int Server_block::get_port() const {}
struct sockaddr_in *Server_block::get_ip_addr() const{
	return (&server_ip);
}
//method for getting all server names
std::vector<std::string> Server_block::get_Snames() const{}
//default constructor add local host and port 80 as basic server block
Server_block::Server_block(){
	server_names.push_back("localhost");
	server_names.push_back("local");
	//port = htons(80);
	server_ip.sin_family = AF_INET;
	server_ip.sin_addr.s_addr = INADDR_ANY;
	server_ip.sin_port = htons(80);

}
//paramertized constructor for init ip and port and server name in one go
Server_block::Server_block(std::string ip, std::string port,std::vector<std::string> server_names){}
//deconstructor for freeing any dynamic allocation
Server_block::~Server_block(){}
//copy constructors could be useful in the future
Server_block::Server_block(const Server_block &obj){}
Server_block & Server_block::operator=(const Server_block &obj){}