#ifndef SERVER_BLOCK_HPP
#define SERVER_BLOCK_HPP

#include "Webserv.hpp"

class Server_block{
	private:
		std::vector<std::string> server_names;
		//map list for location blocks
		std::map<std::string, std::vector<std::string> > location_blocks;
		std::vector<std::string> error_pages;
		bool dir_listen;
		struct sockaddr_in server_ip;
		bool index_flag;
	public:
		int get_port() const;
		struct sockaddr_in *get_ip_addr() ;
		std::vector<std::string> get_Snames() const;
		void set_port(std::string port);
		void set_sname(std::vector <std::string> &vect);
		void set_dir_listen(bool a);
		void set_err_pages(std::vector <std::string> &list);
		void set_ip_addr(std::string ip);
		Server_block();
		Server_block(std::string name_server, int port);
		Server_block(std::string ip, std::string port,std::vector<std::string> server_names);
		~Server_block();
		Server_block(const Server_block &obj);
		Server_block & operator=(const Server_block &obj);

};

#endif