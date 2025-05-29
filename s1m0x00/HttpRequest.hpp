#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <iostream>
#include <string>
#include <stdbool.h>

class HttpRequest{
    public :
        std::string start_line;
        std::string method;
        std::string target;
        std::string version;
        std::string body;
        bool parse(const std::string &raw_request);
    private :
        bool parse_start_line();
};


#endif 