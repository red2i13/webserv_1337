#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <cerrno>

struct Request {
    std::string method;
    std::string query;
    std::string body;
    std::string script_path;
    std::map<std::string, std::string> headers;
};

struct Response {
    std::string raw;
};