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
class SessionManager {
// private:

public:
std::map<std::string, int> sessions;
	std::string get_or_create_session_id(const std::string &client_cookie);
	int track(const std::string &session_id);
	int track_and_get_visit_count(const std::string &session_id);
};

struct Response {
	std::string raw;
	std::string headers;
	std::string body;
};

int handle_cgi(HttpRequest &request, HttpResponse &response, Server_block &f);