#include "../includes/Server_Conf_Parser.hpp"


int Server_Conf_Parser::read_data(){

	std::fstream file(file_name.c_str(), std::fstream::in);
	std::string line;
	while (getline(file , line))
	{
		std::cout << line << std::endl;
	}
	return(0);
}

int Server_Conf_Parser::clean_data(){
	return(0);

}

int Server_Conf_Parser::parse_data(){
	return(0);

}


Server_Conf_Parser::Server_Conf_Parser(){}
Server_Conf_Parser::Server_Conf_Parser(std::string file_path){
	file_name = file_path;
}