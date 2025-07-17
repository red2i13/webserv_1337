#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Server_block.hpp"
#include <ctime>
#include <filesystem> 
// ==== Basic Structs ====
// struct Request {
// 	std::string method;
// 	std::string query;
// 	std::string body;
// 	std::string script_path;
// 	std::map<std::string, std::string> headers;
// };

class SessionManager {
private:
	std::map<std::string, int> sessions;
	int visit_count;
public:
	std::string get_or_create_session_id(const std::string &client_cookie);
	int get_visit_count(const std::string &session_id);
	void increment_visit(const std::string &session_id);
};

struct Response {
	std::string raw;
	std::string headers;
	std::string body;
};

void set_error(int status_code, const std::string &message);
void set_success(int status_code, const std::string &message);
int handle_cgi(HttpRequest &request, HttpResponse &response, Server_block &f);