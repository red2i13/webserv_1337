#include "../includes/HttpResponse.hpp"

HttpResponse::HttpResponse(): status_code(200), status_message("OK"), version("HTTP/1.1"){}



void handle_request(HttpRequest &req, HttpResponse &res) {
    Server_block flag;
    if (req.method == "GET")
        handle_get(req, res, flag);
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

    response << version << " " << status_code << " " << status_message << "\r\n";
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

void handle_get(HttpRequest& req, HttpResponse& res, Server_block& f) {
    std::string path = "./www" + req.target;

    struct stat statbuf; //i check if the file or dir exist
    if (stat(path.c_str(), &statbuf) == -1) {
        res.set_status(404, "Not Found");
        res.set_header("Content-Type", "text/plain");
        res.set_body("404 Not Found");
        return;
    }

    if (S_ISREG(statbuf.st_mode)) {
        // It's a file
        std::ifstream file(path.c_str());
        std::stringstream buffer;
        buffer << file.rdbuf();
        res.set_status(200, "OK");
        std::string mime = get_mime_type(path);
        res.set_header("Content-Type", mime);
        res.set_body(buffer.str());
        res.set_header("Content-Length", to_str(buffer.str().length()));
    } else if (S_ISDIR(statbuf.st_mode)) {
        // It's a directory
        if (!req.target.empty() && req.target[req.target.length() - 1] != '/') {
            res.set_status(301, "Moved Permanently");
            res.set_header("Location", req.target + "/");
            res.set_body("301 Moved Permanently");
            return;
        }

        std::string index_path = path + "/index.html";
        std::ifstream index_file(index_path.c_str());
        if (index_file.is_open()) {
            std::stringstream buffer;
            buffer << index_file.rdbuf();
            res.set_status(200, "OK");
            res.set_header("Content-Type", get_mime_type(index_path));
            res.set_body(buffer.str());
            res.set_header("Content-Length", to_str(buffer.str().length()));
        } 
        else if (f.index_flag){
            // Assume autoindex is enabled (you can replace this with a config flag)
            std::string listing = generate_directory_listing(path, req.target);
            res.set_status(200, "OK");
            res.set_header("Content-Type", "text/html");
            res.set_body(listing);
            res.set_header("Content-Length", to_str(listing.length()));
        }
        else{
            res.set_status(403, "Forbidden");
            res.set_header("Content-Type", "text/plain");
            res.set_body("403 Forbidden");
        }
    } else {
        // Neither file nor dir
        res.set_status(403, "Forbidden");
        res.set_header("Content-Type", "text/plain");
        res.set_body("403 Forbidden");
    }
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


std::string generate_directory_listing(const std::string& path, const std::string& url_path) {
    DIR* dir = opendir(path.c_str());
    if (!dir) return "403 Forbidden";

    std::stringstream ss;
    ss << "<html><body><h1>Index of " << url_path << "</h1><ul>";

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        std::string name = entry->d_name;
        if (name == ".") continue;
        ss << "<li><a href=\"" << name << "\">" << name << "</a></li>";
    }
    closedir(dir);

    ss << "</ul></body></html>";
    return ss.str();
}
