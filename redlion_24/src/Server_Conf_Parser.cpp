#include "../includes/Server_Conf_Parser.hpp"


int Server_Conf_Parser::read_data(){
	//todo return a fail state in a seperate log file when an error occur while reading
	std::fstream file(file_name.c_str(), std::fstream::in);
	std::string line;
	while (getline(file , line))
	{
		f.push_back(line);
		std::cout << line << std::endl;
	}
	if(file.bad())
		return(1);
	if(file.fail())
		return(1);
	return(0);
}
/*
#include <iostream>

int main()
{
    std::string tt = "     hello world";
	size_t gg = tt.find_first_not_of(" \t\n\v\f\r");
	tt.erase(0, gg);
	size_t kk = tt.find_last_not_of(" \t\n\v\f\r");
    std::cout << "the last whitspace index is " << kk << " and size of string " << tt.size() << std::endl;
    tt.erase(kk + 1, tt.size() - 1);
	std::cout << tt << ")" << std::endl;

    return 0;
}
*/
int Server_Conf_Parser::clean_data(){
	for(size_t i  = 0; i < f.size(); i++){
		//skip comment and go to the next line
		if(f[i][0] == '#')
			continue;
		else if(isdigit(f[i][0]))
		{
			//free if memory dynamic and return error
			return(1);
		}
		// else if(f)
	}
	//remove any whitespaces (final step)
	for(size_t i = 0; i < f.size(); i++)	{
		size_t first_non_sp = f[i].find_first_not_of(" \0\t\n\v\f\r");
		if(first_non_sp == std::string::npos){
			f.erase(f.begin() + i);
			i--;
			continue;
		}
		else if(first_non_sp != std::string::npos)
			f[i].erase(0, first_non_sp);
		
		size_t last_non_sp = f[i].find_last_not_of(" \0\t\n\v\f\r");
		f[i].erase(last_non_sp + 1, f[i].size() - 1);
	}
	std::cout << "Test output :\n" << std::endl;
	for(size_t i = 0; i < f.size(); i++)	{
		std::cout << f[i] << std::endl;
	}
	return(0);

}

int parse_data(	std::vector<ConfigNode> &parent, int index){

	for(size_t i = 0; i < f.size(); i++){
		//skip whitespaces
		size_t first_non_sp = f[i].find_first_not_of(" \0\t\n\v\f\r");
		if(first_non_sp == std::string::npos){
			f.erase(f.begin() + i);
			i--;
			continue;
		}
		else if(first_non_sp != std::string::npos)
			f[i].erase(0, first_non_sp);
		
		size_t last_non_sp = f[i].find_last_not_of(" \0\t\n\v\f\r");
		if(last_non_sp != std::string::npos)
			f[i].erase(last_non_sp + 1, f[i].size() - 1);
		//check the comments
		if(f[i].empty() || f[i][0] == '#')
			continue;
		//create token
		std::vector <std::string> tokens;
		std::string token;
		std::istringstream iss(f[i]);

		while(iss >> token){
			tokens.push_back(token);
		}
		if(tokens.empty())
			continue;
		ConfigNode node;
		node.name = tokens[0];
		if(tokens.size() > 1){
			if(tokens.back() == "{"){
				//remove the brace
				parse_data(node.children, index + 1);
			}
			if(tokens.back().back() == ';'){
				
			}
		}
		//process which directive
		// init the blocks classes
	}
	return(0);
}


Server_Conf_Parser::Server_Conf_Parser(){}
Server_Conf_Parser::Server_Conf_Parser(std::string file_path){
	file_name = file_path;
}