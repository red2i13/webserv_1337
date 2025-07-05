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

// ==== Basic Structs ====
struct Request {
	std::string method;
	std::string query;
	std::string body;
	std::string script_path;
	std::map<std::string, std::string> headers;
};

struct Response {
	std::string raw;
	std::string headers;
	std::string body;
};
