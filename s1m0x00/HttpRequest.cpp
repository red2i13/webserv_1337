#include "HttpRequest.hpp"

bool HttpRequest::parse(const std::string &raw_request){
    size_t header_end = raw_request.find("\r\n\r\n");
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
    std::cout<<"========================"<<std::endl;
    std::cout<<"target : "<<target<<std::endl;
    std::cout<<"query : "<<query<<std::endl;
    for (std::map<std::string, std::string>::iterator it = query_param.begin(); it != query_param.end(); ++it) {
    std::cout << it->first << " = " << it->second << std::endl;
    }
    std::cout<<"========================"<<std::endl;
    if (method.empty() || target.empty() || version.empty())
        return false;
    header_part.erase(0, first_crlf + 2);
    size_t pos = 0;

    while((pos = header_part.find("\r\n")) != std::string::npos){
        std::string line = header_part.substr(0, pos);
        header_part.erase(0, pos + 2);
        if (line.empty())
            break;
        //extract header key : value
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
    std::string full_target = this->start_line.substr(first_space + 1, second_space - first_space - 1);
    size_t question_mark = full_target.find('?');
    if (question_mark != std::string::npos) {
        this->target = full_target.substr(0, question_mark);
        this->query = full_target.substr(question_mark + 1);
        extract_query(query);
    } else {
        this->target = full_target;
        this->query = "";
    }
    this->target = url_decode(this->target);
    this->version = this->start_line.substr(second_space + 1);

    return (true);
}

std::string HttpRequest::url_decode(const std::string &str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            std::string hex = str.substr(i + 1, 2);
            char ch = static_cast<char>(std::strtol(hex.c_str(), 0, 16));
            result += ch;
            i += 2;
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}

void HttpRequest::extract_query(const std::string& q) {
    size_t start = 0;
    while (start < q.length()) {
        size_t end = q.find('&', start);
        if (end == std::string::npos)
            end = q.length();

        std::string param = q.substr(start, end - start);
        size_t equal = param.find('=');
        if (equal != std::string::npos) {
            std::string key = param.substr(0, equal);
            std::string value = param.substr(equal + 1);
            query_param[key] = value;
        } else {
            query_param[param] = "";
        }

        start = end + 1;
    }
}




