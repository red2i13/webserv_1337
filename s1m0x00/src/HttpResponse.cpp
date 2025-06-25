#include "../includes/HttpResponse.hpp"

HttpResponse::HttpResponse(): status_code(200), status_message("OK"), version("HTTP/1.1"){}



void handle_request(HttpRequest &req, HttpResponse &res) {
    if (req.method == "GET")
        handle_get(req, res);
    // else if (req.method == "POST")
    //     handle_post(req, res);
    // else if (req.method == "DELETE")
    //     handle_delete(req, res);
    else {
        res.set_status(405, "Method Not Allowed");
        res.set_header("Content-Type", "text/plain");
        res.set_body("405 Method Not Allowed");
    }
}



std::string HttpResponse::to_string() const{
    std::stringstream response;

    response << version << "" << status_code << "" << status_message << "\r\n";
    for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
        response << it->first << ": " << it->second << "\r\n";
    if (headers.find("Content-Length") == headers.end())
        response << "Content-Length: " << body.length() << "\r\n";
    response << "\r\n";
    response << body;
    return (response.str());
}

std::string get_mime_type(const std::string& path) {
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos) return "application/octet-stream";

    std::string ext = path.substr(dot + 1);
    if (ext == "html") return "text/html";
    if (ext == "css") return "text/css";
    if (ext == "js") return "application/javascript";
    if (ext == "png") return "image/png";
    if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
    if (ext == "json") return "application/json";
    if (ext == "txt") return "text/plain";

    return "application/octet-stream";
}


std::string to_str(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}


void handle_get(HttpRequest& req, HttpResponse& res){
    std::string path = "./www"+ req.target;
    
    if (path == "./www/")
        path = "./www/index.html";
    std::ifstream file(path.c_str());
    if (!file.is_open()){
        res.set_status(404, "Not Found");
        res.set_header("Content-Type", "text/plain");
        res.set_body("404 Not Found");
        return;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    res.set_status(200, "OK");
    std::string mime = get_mime_type(path);
    res.set_header("Content-Type", mime);
    res.set_body(buffer.str());
    res.set_header("Content-Length", to_str(buffer.str().length()));
}

void HttpResponse::set_status(int code, const std::string& message) {
    this->status_code = code;
    this->status_message = message;
}

void HttpResponse::set_header(const std::string& key, const std::string& value) {
    this->headers[key] = value;
}

void HttpResponse::set_body(const std::string& body_content) {
    this->body = body_content;
}
