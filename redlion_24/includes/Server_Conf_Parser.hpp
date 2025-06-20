#ifndef SERVER_CONF_PARSER_HPP
#define SERVER_CONF_PARSER_HPP

#include "Webserv.hpp"
#include <fstream>
class Server_Conf_Parser{
	enum{
		DIRECTIVE_NODE,
		BLOCK_NODE,
		VALUE_NODE,
	};
	private:
		std::string file_name;
		std::deque<std::string> f;
	public:
		int read_data();
		int clean_data();
		int parse_data();
		Server_Conf_Parser();
		Server_Conf_Parser(std::string file_path);
};

#endif