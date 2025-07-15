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
std::map<std::string, std::vector<std::string> > Server_block::get_location_blocks() const {
	return location_blocks;
}
void Server_block::set_timeout(int t){
	timeout = t;
}
//default constructor add local host and port 80 as basic server block
Server_block::Server_block(){
	server_names.push_back("localhost");
	server_names.push_back("local");
	server_ip.sin_family = AF_INET;
	server_ip.sin_addr.s_addr = INADDR_ANY;
	server_ip.sin_port = htons(8080);
	index_flag = false;
	upload_flag = false;
	timeout = 60; // Default timeout
	upload_path = "/tmp"; // Default upload path
}
Server_block::Server_block(std::string blank){
	(void)blank;
	index_flag = false;
	upload_flag = false;
	server_ip.sin_family = AF_INET;
	server_ip.sin_addr.s_addr = INADDR_ANY;
	timeout = 60; // Default timeout
	upload_path = "/tmp"; // Default upload path
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
void Server_block::set_ip_host(std::vector <std::string> &vect){
	//TODO add a check for the ip address if its valid
	if(vect.size() == 0 || vect.size() > 2){
		std::cout << "Invalid ip address, using default ip address" << std::endl;
		return;
	}
	//put the address in the server_ip struct
	if(vect.size() == 1){
		//find the port after : and set the port
		size_t pos = vect[0].find(':');
		if(pos != std::string::npos){
			server_ip.sin_port = htons(atoi(vect[0].substr(pos + 1).c_str()));
			server_ip.sin_addr.s_addr = inet_addr(vect[0].substr(0, pos).c_str());
		}
		else{
			server_ip.sin_port = htons(8080); // Default port if not specified
			server_ip.sin_addr.s_addr = inet_addr(vect[0].c_str());
		
		}
	}
}
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

void Server_block::set_location(std::string directory, std::vector <std::string> &list){
	//TODO add a check for the location if its valid
	if(list.size() > 0){
		std::string loc = directory;
		location_blocks[loc] = std::vector<std::string>();
		for(size_t i = 0; i < list.size(); i++){
			location_blocks[loc].push_back(list[i]);
		}
	}
	else{
		std::cout << "Invalid location, using default location" << std::endl;
	}
}

void Server_block::set_upload_path(std::vector<std::string> &list) {
	upload_flag = true;
	if (list.size() == 1) {
		upload_path = list[0];
	} else {
		std::cout << "Invalid upload path, using default path" << std::endl;
	}
}