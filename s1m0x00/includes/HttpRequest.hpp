#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <iostream>
#include <string>
#include <stdbool.h>
#include <cstdlib>
#include <sstream>
#include <map>

class HttpRequest{
    public :
        HttpRequest();
        std::string start_line;
        std::string method;
        std::string target;
        std::string query;
        // bool cgi_flag;
        std::map<std::string, std::string> query_param;
        std::map<std::string, std::string> headers;
        std::string version;
        std::string body;
        bool parse(const std::string &raw_request);
        std::string url_decode(const std::string &str);
        void extract_query(const std::string& q);
        std::string trim(const std::string &str);
    private :
        bool parse_start_line();
};


#endif 