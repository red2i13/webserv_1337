#ifndef SERVER_BLOCK_HPP
#define SERVER_BLOCK_HPP

#include "Webserv.hpp"

class Server_block{
	private:
		std::vector<std::string> server_names;
		struct sockaddr_in server_ip;
	public:
		int get_port() const;
		struct sockaddr_in *get_ip_addr() const;
		std::vector<std::string> get_Snames() const;
		Server_block();
		Server_block(std::string ip, std::string port,std::vector<std::string> server_names);
		~Server_block();
		Server_block(const Server_block &obj);
		Server_block & operator=(const Server_block &obj);

};

#endif