#include "../includes/HttpResponse.hpp"

HttpResponse::HttpResponse(): status_code(200), status_message("OK"), version("HTTP/1.1"){}


std::string to_str(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string get_error_page(int code) {
    std::string filepath = "www/error_pages/" + to_str(code) + ".html";
    std::ifstream file(filepath.c_str());

    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    // Fallback default message
    std::stringstream fallback;
    fallback << "<!DOCTYPE html>\n"
             << "<html><head><title>" << code << " Error</title></head><body>"
             << "<style>body{text-align: center;}</style>"
             << "<h1>" << code << " Error</h1>"
             << "<p>The server encountered an error and could not complete your request.</p>"
             << "</body></html>";
    return (fallback.str());
}


void handle_request(HttpRequest &req, HttpResponse &res, Server_block &flag) {
    
    if (req.method == "GET")
        handle_get(req, res, flag);
    else if (req.method == "POST")
    {
        std::cout<<"im a here!"<<std::endl;
        handle_post(req, res, flag);
    }
    else if (req.method == "DELETE")
        handle_delete(req, res);
    else {
        res.set_status(405, "Method Not Allowed");
        res.set_header("Content-Type", "text/html");
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


void handle_get(HttpRequest& req, HttpResponse& res, Server_block& f) {
    std::string path = "./www" + req.target;

    struct stat statbuf; //i check if the file or dir exist
    if (stat(path.c_str(), &statbuf) == -1) {
        res.set_status(404, "Not Found");
        res.set_header("Content-Type", "text/html");
        res.set_body(get_error_page(404));
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
            res.set_body(get_error_page(301));
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
            res.set_header("Content-Type", "text/html");
            res.set_body(get_error_page(403));
        }
    } else {
        // Neither file nor dir
        res.set_status(403, "Forbidden");
        res.set_header("Content-Type", "text/html");
        res.set_body(get_error_page(403));
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

void handle_post(HttpRequest& req, HttpResponse& res, Server_block& f){
    std::cout << f.upload_flag << std::endl;
    if (!f.upload_flag)
    {
        res.set_status(403, "Forbidden");
        res.set_header("Content-Type", "text/html");
        res.set_body(get_error_page(403));
        return;
    }
    std::string body_data;
    std::string content_type = req.headers["content-type"];
    if (content_type.find("multipart/form-data") != std::string::npos) {
        std::string boundary;
        size_t pos = content_type.find("boundary=");
        if (pos != std::string::npos)
            boundary = "--" + content_type.substr(pos + 9); // skip "boundary="

        handle_multipart_form(body_data, boundary, res);
        return;
    }

    if (req.headers.count("content-length"))
    {
        size_t content_length = atoi(req.headers["content-length"].c_str());
        std::cout << "debug size content " << content_length << std::endl;
        body_data = req.body.substr(0, content_length);
    } 
    else if (req.headers.count("transfer-encoding") && req.headers["transfer-encoding"] == "chunked")
    {
        body_data = decode_chunked_body(req.body);
        if (body_data.empty())
        {
            res.set_status(400, "Bad Request");
            res.set_body(get_error_page(400));

            return;
        }
    }
    else
    {
        res.set_status(411, "Length Required");
        res.set_header("Content-Type", "text/html");
        res.set_body(get_error_page(411));

        return;
    }
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != 0) {
        f.upload_path = cwd;
    }
    std::string filename = "upload.txt";
    std::string full_path = f.upload_path + "/" + filename;
    std::ofstream file(full_path.c_str(), std::ios::binary);
    if (!file) {
        res.set_status(500, "Internal Server Error");
        res.set_body(get_error_page(500));
        return;
    }

    file << body_data;
    file.close();

    res.set_status(201, "Created");
    res.set_header("Content-Type", "text/html");
    res.set_body("Upload successful");
}

std::string decode_chunked_body(const std::string& raw) {
    std::stringstream input(raw);
    std::string decoded;
    std::string line;

    while (std::getline(input, line)) {
        size_t chunk_size = std::strtoul(line.c_str(), 0, 16);
        if (chunk_size == 0)
            break;

        char* buffer = new char[chunk_size];
        input.read(buffer, chunk_size);
        decoded.append(buffer, chunk_size);
        delete[] buffer;

        input.ignore(2); // skip \r\n after chunk
    }

    return decoded;
}

void handle_delete(HttpRequest &req, HttpResponse &res) {
    std::string path = "./www" + req.target;
    struct stat s;
    if (stat(path.c_str(), &s) != 0) {
        res.set_status(404, "Not Found");
        res.set_body(get_error_page(404));
        return;
    }

    if (S_ISREG(s.st_mode)) {
        if (unlink(path.c_str()) != 0) {
            res.set_status(500, "Internal Server Error");
            res.set_body(get_error_page(500));
            return;
        }
        res.set_status(204, "No Content"); // File deleted
        return;
    }

    if (S_ISDIR(s.st_mode)) {
        // URI must end with a slash for directories
        if (req.target.empty() || req.target[req.target.size() - 1] != '/') {
            res.set_status(409, "Conflict");
            res.set_body(get_error_page(409));

            return;
        }
        if (access(path.c_str(), W_OK) != 0) {
            res.set_status(403, "Forbidden");
            res.set_body(get_error_page(403));
            return;
        }
        if (rmdir(path.c_str()) != 0) {
            res.set_status(500, "Internal Server Error");
            res.set_body(get_error_page(500));
            return;
        }
        res.set_status(204, "No Content"); // Dir deleted
        return;
    }

    res.set_status(500, "Internal Server Error");
    res.set_body(get_error_page(500));
}


void handle_multipart_form(const std::string& body, const std::string& boundary, HttpResponse& res) {
    size_t start = 0;
    size_t end = 0;

    while ((start = body.find(boundary, end)) != std::string::npos) {
        start += boundary.length() + 2; // Skip boundary and CRLF
        end = body.find(boundary, start);
        if (end == std::string::npos)
            break;

        std::string part = body.substr(start, end - start);
        
        // Separate headers and content
        size_t header_end = part.find("\r\n\r\n");
        if (header_end == std::string::npos)
            continue;

        std::string headers = part.substr(0, header_end);
        std::string content = part.substr(header_end + 4);

        // Look for filename
        size_t fn_pos = headers.find("filename=\"");
        if (fn_pos != std::string::npos) {
            fn_pos += 10;
            size_t fn_end = headers.find("\"", fn_pos);
            std::string filename = headers.substr(fn_pos, fn_end - fn_pos);

            char cwd[PATH_MAX];
            std::string upload_path;
            if (getcwd(cwd, sizeof(cwd)) != 0)
                upload_path = cwd;
            
            std::string full_path = upload_path + "/" + filename;
            std::cout << "full path ::: "<< full_path <<std::endl;
            std::ofstream out(full_path.c_str(), std::ios::binary);
            if (!out) {
                res.set_status(500, "Internal Server Error");
                res.set_body(get_error_page(500));
                return;
            }
            out << content;
            out.close();
        }
    }

    res.set_status(201, "Created");
    res.set_header("Content-Type", "text/html");
    res.set_body("File uploaded successfully.");
}
