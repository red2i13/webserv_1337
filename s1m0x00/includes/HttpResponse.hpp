#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HttpRequest.hpp"
#include "Webserv.hpp"
#include <fstream>

class HttpResponse{
    public :
        HttpResponse();
        int status_code;
        std::string status_message;
        std::string version;
        std::map<std::string, std::string> headers;
        std::string body;
        std::string to_string() const;
        void set_status(int code, const std::string& message);
        void set_header(const std::string& key, const std::string& value);
        void set_body(const std::string& body_content);
};
void handle_request(HttpRequest &req, HttpResponse &res);
void handle_get(HttpRequest& req, HttpResponse& res);
#endif