#ifndef SERVER_BLOCK_HPP
#define SERVER_BLOCK_HPP

#include "Webserv.hpp"
#include "../includes/Server_Conf_Parser.hpp"

//a location struct for storing the respective path and methods and max body size
struct Location {
	Location() : max_body_size(0), autoindex(false) , cgi_flag(false) {}
	std::string path;
	std::vector<std::string> allowed_methods;
	size_t max_body_size; // in bytes
	bool autoindex; // true if directory listing is enabled
	bool cgi_flag; // true if CGI is enabled for this location
	std::string upload_path; // path for file uploads
};

//


class Server_block{
	private:
		std::vector<std::string> server_names;
		//map list for location blocks
		std::map<std::string, std::vector<Location> > location_blocks;
		std::vector<std::string> error_pages;
		struct sockaddr_in server_ip;
		std::string root_path;
	public:
		int timeout;
		bool upload_flag;
		bool index_flag;
		std::string upload_path;
		struct sockaddr_in *get_ip_addr() ;
		std::string get_root_path() const;
		std::vector<std::string> get_error_pages() const;
		std::map<std::string, std::vector<Location> > get_locations_blocks() const ;		
		std::vector<Location> get_location_block(std::string loc) const ;		
		std::vector<std::string> get_Snames() const;
		void set_sname(std::vector <std::string> &vect);
		void set_dir_listen(bool a);
		void set_err_pages(std::vector <std::string> &list);
		void set_ip_host(std::vector <std::string> &vect);
		void set_location(std::string directory, std::vector<ConfigNode> &children);
		void set_upload_path(std::vector<std::string> &list) ;
		void set_root_path(const std::string &path);
		void set_timeout(int t);
		Server_block();
		Server_block(std::string blank);
		// Server_block(std::string name_server, int port);
		~Server_block();
		Server_block(const Server_block &obj);
		Server_block & operator=(const Server_block &obj);

};

#endif