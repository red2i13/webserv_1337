#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HttpRequest.hpp"
#include "Webserv.hpp"
#include "Server_block.hpp"
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>

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
        /////////////
        void set_error(int code, const std::string& message, const std::string& extra = "");
        void set_success(int code, const std::string& message);
        ///////////////
    };

std::string int_to_string(int n);
void handle_request(HttpRequest &req, HttpResponse &res, Server_block &flag);
void handle_get(HttpRequest& req, HttpResponse& res, Server_block& f);
void handle_post(HttpRequest& req, HttpResponse& res, Server_block& f);
std::string generate_directory_listing(const std::string& path, const std::string& url_path);
std::string decode_chunked_body(const std::string& raw);
void handle_delete(HttpRequest& req, HttpResponse& res, Server_block& f);
void handle_multiple_form(const std::string& body, const std::string& boundary, HttpResponse& res, const std::string& upload_dir, Server_block& f);
#endif