#include "HttpRequest.hpp"

bool HttpRequest::parse(const std::string &raw_request){
    ssize_t header_end = raw_request.find("\r\n\r\n");
    if (header_end != std::string::npos){
        std::cout<<"1 parse err"<<std::endl;
        return (false);
    }
    std::string header_part = raw_request.substr(0, header_end);
    std::string 
}