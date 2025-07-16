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
        handle_post(req, res, flag);
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

std::string extract_directory_from_target(const std::string& target) {
    size_t last_slash = target.rfind('/');
    if (last_slash == std::string::npos)
        return "/";
    if (last_slash == target.length() - 1)
        return target;
    return target.substr(0, last_slash);
}


void handle_get(HttpRequest& req, HttpResponse& res, Server_block& f) {
    //search for the correct file in location map
    //here
    // std::string location = extract_directory_from_target(req.target);
    // std::cout << "GET request for: " << req.target << std::endl;
    // std::map<std::string, std::vector<std::string> > location_blocks = f.get_location_blocks();
    // std::string path;// =  + req.target;
    // std::map<std::string, std::vector<std::string> >::iterator it = location_blocks.begin();
    // for (; it != location_blocks.end(); it++) {
    //     if (it->first == location) {
    //         std::cout << "Found location block for: " << req.target << std::endl;
    //         std::cout << it->first << " -> " << it->second[0] << std::endl;
    //         path = f.get_root_path() + "/" + it->second[0] + req.target;
    //         break;
    //     }
    // }

    // if(it != location_blocks.end()) {
    //     std::cout << "Found location block for: " << req.target << std::endl;
    //     std::cout << it->first << " -> " << it->second[0] << std::endl;
    //     path = f.get_root_path() + "/" + it->second[0] + req.target;
    //     // Handle the request based on the location block
    //     // For example, you can set headers or modify the response based on the location block
    // } else {
    //     std::cout << "No specific location block found for: " << req.target << std::endl;
    //     path = f.get_root_path() + req.target;
    //     std::cout << "Using root path: " << path << std::endl;
    // }

    std::string location = extract_directory_from_target(req.target);
    std::map<std::string, std::vector<std::string> > location_blocks = f.get_location_blocks();
    std::string path;

    std::string root_path = f.get_root_path();
    std::string target = req.target;

    std::string base_dir = root_path;
    if (location_blocks.count("/") > 0 && !location_blocks["/"].empty()) {
        base_dir += "/" + location_blocks["/"][0];
    }

    // Overwrite with more specific location if it exists
    if (location_blocks.count(location) > 0 && !location_blocks[location].empty()) {
        base_dir = root_path + "/" + location_blocks[location][0];
    }

    if (!base_dir.empty() && base_dir[base_dir.length() - 1] == '/' &&
        !target.empty() && target[0] == '/')
    {
        path = base_dir + target.substr(1);
    }
    else
    {
        path = base_dir + target;
    }


    std::cout << "Target: " << target << std::endl;

    std::cout << "GET request for: " << target << std::endl;
    std::cout << "Resolved path: " << path << std::endl;



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

void handle_post(HttpRequest& req, HttpResponse& res, Server_block& f){
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
        
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != 0)
        f.upload_path = cwd;
        
        body_data = req.body;
        handle_multiple_form(body_data, boundary, res, f.upload_path);
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
    // char cwd[PATH_MAX];
    // if (getcwd(cwd, sizeof(cwd)) != 0) {
    //     f.upload_path = cwd;
    // }
    std::cout << "---------------------Upload path: " << f.upload_path << std::endl;
    std::string filename = "/upload.txt";
    std::string full_path = f.upload_path + filename;
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


void handle_multiple_form(const std::string& body, const std::string& boundary, HttpResponse& res, const std::string& upload_dir) {
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
            std::string full_path = upload_dir + "/www/upload/" + filename;
            std::cout<< full_path << std::endl;
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