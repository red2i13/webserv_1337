#include "../includes/HttpResponse.hpp"

HttpResponse::HttpResponse(): status_code(200), status_message("OK"), version("HTTP/1.1"){}


std::string to_str(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string get_error_page(int code, Server_block &f) {
    std::string filepath = f.get_root_path() + f.get_error_pages().back() + "/" + to_str(code) + ".html";
    // std::cout << "Looking for error page at: " << filepath << std::endl;
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
    Location loc = flag.get_location_block(req.target);
    std::vector<std::string> list =  loc.allowed_methods;
    std::cout << req.method << std::endl;
    for(std::vector<std::string>::iterator it = list.begin(); it != list.end(); ++it) {
        if (req.method == *it && req.method == "GET") {
            handle_get(req, res, flag, loc.path);
            return;

        }
        else if (req.method == *it && req.method == "POST"){
            std::cout << "+++++++++++++++" << std::endl;
            handle_post(req, res, flag, loc.upload_path);
            return;
        }
        else if (req.method == *it && req.method == "DELETE"){
            handle_delete(req, res, flag, loc.path);
            return;
        }
    }
    res.set_status(405, "Method Not Allowed");
    res.set_header("Content-Type", "text/html");
    res.set_body("405 Method Not Allowed");
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

std::string extract_directory_from_target(const std::string& target) {
    size_t last_slash = target.rfind('/');
    if (last_slash == std::string::npos)
        return "/";
    if (last_slash == target.length() - 1)
        return target;
    return target.substr(0, last_slash);
}


void handle_get(HttpRequest& req, HttpResponse& res, Server_block& f, std::string location) {
    (void)location;
    std::string path;
    Location locations = f.get_location_block(req.target);


    if (!locations.redirect.empty()) {
        if (req.redirect_count >= 5) {
            std::cout << "Too Many Redirects" << std::endl;
            res.set_status(500, "Too Many Redirects");
            res.set_body(get_error_page(500, f));
            return;
        }
        if (req.target == locations.redirect || locations.redirect == req.target + "/") {
            res.set_status(500, "Redirect Loop Detected");
            res.set_body(get_error_page(500, f));
            return;
        }
        req.redirect_count++;
        std::cout << "redir_count: " << req.redirect_count << std::endl;    
        // res.set_status(301, "Moved Permanently");
        // res.set_header("Location", locations.redirect);
        // res.set_body("<html><body><h1>301 Moved</h1><p><a href=\"" + locations.redirect + "\">Moved here</a></p></body></html>");
        Location next = f.get_location_block(locations.redirect);
        std::cout << "Next location: " << next.path << std::endl;
        std::cout << "Redirecting to: " << locations.redirect << std::endl;
        req.target = locations.redirect;
        handle_get(req, res, f, locations.redirect);
        return;
    }

    std::string root_path;
    if (locations.path.empty()) {
        root_path = f.get_root_path();
    } else {
        root_path = locations.path;
    }
    
    std::string target = req.target;

    std::string base_dir = root_path;
        path = base_dir + target;
    struct stat statbuf; //i check if the file or dir exist
    if (stat(path.c_str(), &statbuf) == -1) {
        res.set_status(404, "Not Found");
        res.set_header("Content-Type", "text/html");
        res.set_body(get_error_page(404, f));
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
            res.set_body(get_error_page(301, f));
            return;
        }

        std::string index_path = path + locations.index;
        std::ifstream index_file(index_path.c_str());
        if (index_file.is_open()) {
            std::stringstream buffer;
            buffer << index_file.rdbuf();
            res.set_status(200, "OK");
            res.set_header("Content-Type", get_mime_type(index_path));
            res.set_body(buffer.str());
            res.set_header("Content-Length", to_str(buffer.str().length()));
        } 
        else if (locations.autoindex){
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
            res.set_body(get_error_page(403, f));
        }
    } else {
        // Neither file nor dir
        res.set_status(403, "Forbidden");
        res.set_header("Content-Type", "text/html");
        res.set_body(get_error_page(403, f));
    }
}
////////////////
std::string int_to_string(int n) {
	std::stringstream ss;
	ss << n;
	return ss.str();
}

void HttpResponse::set_error(int code, const std::string& message, const std::string& extra) {
	this->set_status(code, message);
	this->set_header("Content-Type", "text/html");
	this->set_body("<html><body><h1>" + int_to_string(code) + " " + message + "</h1><p>" + extra + "</p></body></html>");
}

void HttpResponse::set_success(int code, const std::string& message) {
	this->set_status(code, message);
	this->set_header("Content-Type", "text/html");
	this->set_body("<html><body><h1>" + int_to_string(code) + " " + message + "</h1></body></html>");
}
////////////////

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

void handle_post(HttpRequest& req, HttpResponse& res, Server_block& f, std::string location = "") {
    // std::string location = extract_directory_from_target(req.target);
    Location locations = f.get_location_block(location);
    std::cout << "LOOOOCATION  " << locations.upload_path << std::endl;

    if (locations.upload_path.empty())
    {
        res.set_status(403, "Forbidden");
        res.set_header("Content-Type", "text/html");
        res.set_body(get_error_page(403, f));
        return;
    }
    std::string body_data;
    std::string content_type = req.headers["content-type"];
    if (content_type.find("multipart/form-data") != std::string::npos) {
        std::string boundary;
        size_t pos = content_type.find("boundary=");
        if (pos != std::string::npos)
        boundary = "--" + content_type.substr(pos + 9);
        body_data = req.body;
        if (body_data.size() > locations.max_body_size) {
            res.set_header("Content-Type", "text/html");
            res.set_status(413, "Payload Too Large");
            res.set_body(get_error_page(413, f));
            return;
        }

        if (!boundary.empty())
            handle_multiple_form(body_data, boundary, res, locations.upload_path, f);
        else {
            res.set_status(400, "Bad Request");
            res.set_body(get_error_page(400, f));
        }
    }

    if (req.headers.count("transfer-encoding") && req.headers["transfer-encoding"] == "chunked")
    {
        // std::cout << "Handling chunked transfer encoding" << std::endl;
        body_data = decode_chunked_body(req.body);
        if (body_data.empty())
        {
            res.set_status(400, "Bad Request");
            res.set_body(get_error_page(400, f));

            return;
        }
    }
    else if (req.headers.count("transfer-encoding") && req.headers["transfer-encoding"] != "chunked")
    {
        res.set_status(400, "Bad Request");
        res.set_header("Content-Type", "text/html");
        res.set_body(get_error_page(400, f));
        return;
    }
    else if (req.headers.count("content-length"))
    {
        size_t content_length = atoi(req.headers["content-length"].c_str());
        // std::cout << "debug size content " << req.body << std::endl;
        body_data = req.body.substr(0, content_length);
    } 
    else
    {
        res.set_status(411, "Length Required");
        res.set_header("Content-Type", "text/html");
        res.set_body(get_error_page(411, f));

        return;
    }
    
    std::cout << "---------------------Upload path: " << locations.upload_path << std::endl;
    std::string filename = "/upload.txt";
    std::string full_path = locations.upload_path + filename;
    // std::cout << "Full path: " << full_path << std::endl;
    std::ofstream file(full_path.c_str(), std::ios::binary);
    if (!file) {
        res.set_status(500, "Internal Server Error");
        res.set_body(get_error_page(500, f));
        return;
    }
    // std::cout << "=============" << body_data << std::endl;
    file << body_data;
    file.close();

    res.set_status(201, "Created");
    res.set_header("Content-Type", "text/html");
    res.set_body(get_error_page(201, f));
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

void handle_delete(HttpRequest &req, HttpResponse &res, Server_block &f, std::string location) {
    // std::string location = extract_directory_from_target(req.target);
    Location locations = f.get_location_block(location);
    std::string path;
    if (locations.path.empty())
        path = "./www" + req.target;
    else
        path = locations.path + req.target;
    struct stat s;
    if (stat(path.c_str(), &s) != 0) {
        res.set_status(404, "Not Found");
        res.set_body(get_error_page(404, f));
        return;
    }
    if (access(path.c_str(), W_OK) != 0) {
        res.set_status(403, "Forbidden");
        res.set_body(get_error_page(403, f));
        return;
    }

    if (S_ISREG(s.st_mode)) {
        if (unlink(path.c_str()) != 0) {
            res.set_status(500, "Internal Server Error");
            res.set_body(get_error_page(500, f));
            return;
        }
        res.set_status(204, "No Content"); // File deleted
        return;
    }

    if (S_ISDIR(s.st_mode)) {
        // URI must end with a slash for directories
        if (req.target.empty() || req.target[req.target.size() - 1] != '/') {
            res.set_status(409, "Conflict");
            res.set_body(get_error_page(409, f));

            return;
        }
        if (access(path.c_str(), W_OK) != 0) {
            res.set_status(403, "Forbidden");
            res.set_body(get_error_page(403, f));
            return;
        }
        if (rmdir(path.c_str()) != 0) {
            res.set_status(500, "Internal Server Error");
            res.set_body(get_error_page(500, f));
            return;
        }
        res.set_status(204, "No Content"); // Dir deleted
        return;
    }

    res.set_status(500, "Internal Server Error");
    res.set_body(get_error_page(500, f));
}

void handle_multiple_form(const std::string& body, const std::string& boundary, HttpResponse& res, const std::string& upload_dir, Server_block& f) {
    size_t start = 0;
    size_t end = 0;
    while ((start = body.find(boundary, end)) != std::string::npos) {
        start += boundary.length() + 2; // Skip boundary + CRLF
        end = body.find(boundary, start);
        std::cout<< "uploaaaad" <<std::endl;
        if (end == std::string::npos)
        break;
        
        std::string part = body.substr(start, end - start);
        
        // Separate headers and content
        size_t header_end = part.find("\r\n\r\n");
        if (header_end == std::string::npos)
        continue;
        
        std::string headers = part.substr(0, header_end);
        std::string content = part.substr(header_end + 4);

        // Find filename
        size_t fn_pos = headers.find("filename=\"");
        if (fn_pos != std::string::npos) {
            fn_pos += 10;
            size_t fn_end = headers.find("\"", fn_pos);
            std::string filename = headers.substr(fn_pos, fn_end - fn_pos);

            // Trim trailing CRLF
            if (!content.empty() && content[content.size() - 1] == '\n') {
                if (content.size() > 1 && content[content.size() - 2] == '\r')
                    content = content.substr(0, content.size() - 2);
                else
                    content = content.substr(0, content.size() - 1);
            }

            // Write to upload directory
            std::string full_path = upload_dir + filename;
            // std::cout<< full_path << std::endl;
            std::ofstream out(full_path.c_str(), std::ios::binary);
            if (!out) {
                res.set_status(500, "Internal Server Error");
                res.set_body(get_error_page(500, f));
                return;
            }
            out << content;
            out.close();
        }
    }

    res.set_status(201, "Created");
    res.set_header("Content-Type", "text/html");
    res.set_body(get_error_page(201, f));
}