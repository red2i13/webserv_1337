#include "HttpRequest.hpp"

bool HttpRequest::parse(const std::string &raw_request){
    ssize_t header_end = raw_request.find("\r\n\r\n");
    if (header_end == std::string::npos){
        std::cout<<"1 parse err"<<std::endl;
        return (false);
    }
    std::string header_part = raw_request.substr(0, header_end);
    std::string body_part = raw_request.substr(header_end + 4);
    
    size_t first_crlf = header_part.find("\r\n");
    this->start_line = header_part.substr(0, first_crlf);
    if (!parse_start_line())
        return (false);
    if (method.empty() || target.empty() || version.empty())
        return false;
    header_part.erase(0, first_crlf + 2);
    ssize_t pos = 0;

    while((pos = header_part.find("\r\n")) != std::string::npos){
        std::string line = header_part.substr(0, pos);
        header_part.erase(0, pos + 2);
        if (line.empty())
            break;
    }
    return (true);
}

bool HttpRequest::parse_start_line(){
    size_t first_space = this->start_line.find(' ');
    if (first_space == std::string::npos)
        return false;

    this->method = this->start_line.substr(0, first_space);

    size_t second_space = this->start_line.find(' ', first_space + 1);
    if (second_space == std::string::npos)
        return false;

    this->target = this->start_line.substr(first_space + 1, second_space - first_space - 1);
    this->version = this->start_line.substr(second_space + 1);
    return (true);
}

