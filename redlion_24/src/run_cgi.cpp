#include "../includes/run_cgi.hpp"


// // ==== session part ====
// static SessionManager sessionManager;


// std::string SessionManager::get_or_create_session_id(const std::string &client_cookie) {
// 	if (client_cookie.find("session_id=") != std::string::npos) {
// 		size_t start = client_cookie.find("session_id=") + 11;
// 		size_t end = client_cookie.find(";", start);
// 		return client_cookie.substr(start, end - start);
// 	}
// 	std::stringstream ss;
// 	ss << std::time(0);
// 	std::string new_id = ss.str();
// 	sessions[new_id] = 1;
// 	return new_id;
// }

// int SessionManager::get_visit_count(const std::string &session_id) {
// 	return sessions[session_id];
// }

// void SessionManager::increment_visit(const std::string &session_id) {
// 	sessions[session_id]++;
// }

// ==== Main CGI Handler ====
// int handle_cgi(HttpRequest &request, HttpResponse &response, Server_block &f) {
// 	std::string script_path = "./www" + request.target;
// 	// CHECK METHID
// 	if (request.method == "DELETE") 
// 	{
// 		if (access(script_path.c_str(), F_OK) == -1) {
// 			response.set_status(404, "Not Found");
// 			response.set_header("Content-Type", "text/html");
// 			response.set_body("<html><body><h1>404 Not Found</h1><p>File to delete not found.</p></body></html>");
// 			return 1;
// 		}
// 		if (unlink(script_path.c_str()) == -1) {
// 			response.set_status(500, "Internal Server Error");
// 			response.set_header("Content-Type", "text/html");
// 			response.set_body("<html><body><h1>500 Internal Server Error</h1><p>Cannot delete file.</p></body></html>");
// 			return 1;
// 		}
// 		response.set_status(204, "No Content");
// 		response.set_header("Content-Type", "text/html");
// 		response.set_body(""); // No body for 204
// 		return 0;
// 	}
// 	// CHECK PATH
// 	if (request.target.empty()) {
// 		response.set_status(400, "Bad Request");
// 		response.set_header("Content-Type", "text/html");
// 		response.set_body("<html><body><h1>400 Bad Request</h1><p>Script path is empty.</p></body></html>");
// 		return 1;
// 	}
// 	else if (access(script_path.c_str(), F_OK) == -1) {
// 		response.set_status(404, "Not Found");
// 		response.set_header("Content-Type", "text/html");
// 		response.set_body("<html><body><h1>404 Not Found</h1><p>CGI script not found.</p></body></html>");
// 		return 1;
// 	}
// 	else if (access(script_path.c_str(), X_OK) == -1) {
// 		response.set_status(403, "Forbidden");
// 		response.set_header("Content-Type", "text/html");
// 		response.set_body("<html><body><h1>403 Forbidden</h1><p>CGI script is not executable.</p></body></html>");
// 		return 1;
// 	}
// 	int fd_in[2], fd_out[2];
// 	if (pipe(fd_in) == -1 || pipe(fd_out) == -1) {
// 		write(2, "pipe\n", 5);
// 		return 1;
// 	}
	
// 	pid_t pid = fork();
// 	if (pid == -1) {
// 		write(2, "fork\n", 5);
// 		return 1;
// 	}
	
// 	if (pid == 0) {
// 		// CHILD PROCESS
// 		dup2(fd_in[0], STDIN_FILENO);
// 		dup2(fd_out[1], STDOUT_FILENO);
// 		close(fd_in[1]);
// 		close(fd_out[0]);
		
// 		// Build ENV
// 		std::vector<std::string> env_strings;
// 		env_strings.push_back("REQUEST_METHOD=" + request.method);
// 		std::cout << "REQUEST_METHOD: " << request.method << std::endl;
// 		env_strings.push_back("REQUEST_URI=" + request.target);
// 		env_strings.push_back("HTTP_COOKIE=" + request.cookies);	
// 		// std::cout << " <<<<<<<<<<< i'm here >>>>>>>>>>>>>" << std::endl;
// 		std::cout << "HTTP_COOKIE: " << request.cookies << std::endl;
// 		env_strings.push_back("HTTP_USER_AGENT=" + request.headers["user-Agent"]);
// 		//std::cout << "HTTP_USER_AGENT: " << request.headers["User-Agent"] << std::endl;
// 		env_strings.push_back("SERVER_NAME=localhost");
// 		env_strings.push_back("SERVER_PORT=8080");	
// 		env_strings.push_back("QUERY_STRING=" + request.query);
// 		env_strings.push_back("CONTENT_LENGTH=" + int_to_string(request.body.size()));
// 		std::map<std::string, std::string>::const_iterator it = request.headers.find("Content-Type");
// 		if (it != request.headers.end())
// 			env_strings.push_back("CONTENT_TYPE=" + it->second);	
// 		env_strings.push_back("GATEWAY_INTERFACE=CGI/1.1");
// 		env_strings.push_back("SCRIPT_NAME=" + request.target);
// 		env_strings.push_back("SERVER_PROTOCOL=HTTP/1.1");
// 		env_strings.push_back("REDIRECT_STATUS=200");
		
// 		env_strings.push_back("SCRIPT_FILENAME=" + script_path);
// 		env_strings.push_back("PATH_INFO=" + request.target);
// 		env_strings.push_back("REQUEST_URI=" + request.target);
		
		
// 		std::vector<char*> envp;
// 		for (size_t i = 0; i < env_strings.size(); ++i)
// 		envp.push_back(strdup(env_strings[i].c_str()));
// 		envp.push_back(NULL);
		
// 		std::string ext = script_path.substr(script_path.find_last_of('.'));
// 		std::string interpreter;
		
// 		if (ext == ".py") interpreter = "/usr/bin/python3";
// 		else if (ext == ".php") interpreter = "/usr/bin/php-cgi";
// 		else interpreter = "";
// 		if(access(interpreter.c_str(), F_OK) == -1) {
// 			std::cout << "NO FILE" << std::endl;			
// 			exit (5); // No interpreter found, will try to exec script directly
// 		}
// 		char *argv[] = { NULL, NULL, NULL };
// 		if (!interpreter.empty()) {
// 			argv[0] = strdup(interpreter.c_str());
// 			argv[1] = strdup(script_path.c_str());
// 			argv[2] = NULL;
// 			execve(argv[0], argv, &envp[0]);
// 		} else {
// 			argv[0] = strdup(script_path.c_str());
// 			argv[1] = NULL;
// 			execve(argv[0], argv, &envp[0]);
// 		}
// 		perror("execve failed");
		
// 		// Free memory only if execve fails
// 		for (size_t i = 0; i < envp.size() - 1; ++i)
// 		free(envp[i]);
		
// 		for (int i = 0; argv[i]; ++i)
// 		free(argv[i]);
		
// 		exit(1);
// 	} 
// 	else {
// 		// PARENT PROCESS
// 		close(fd_in[0]);
// 		close(fd_out[1]);
		
// 		if (request.method == "POST")
// 		{
// 			handle_post(request,response, f); // Assuming blocks[0] is the default server block
// 			write(fd_in[1], request.body.c_str(), request.body.size());
// 		}
// 		else if (request.method == "GET")
// 		{
// 			write(fd_in[1], request.body.c_str(), request.body.size());
// 		}
		
// 		close(fd_in[1]);
		
// 		char buffer[1024];
// 		std::string output;
// 		ssize_t n;
// 		while ((n = read(fd_out[0], buffer, sizeof(buffer))) > 0)
// 			output.append(buffer, n);
// 		std::cout << response.status_code << response.status_message << std::endl;
// 		close(fd_out[0]);
// 		waitpid(pid, NULL, 0);
// 		// Split headers and body
// 		size_t header_end = output.find("\r\n\r\n");
// 		if (header_end != std::string::npos) {
// 			std::string header_block = output.substr(0, header_end);
// 			std::istringstream header_stream(header_block);
// 			std::string line;

// 			while (std::getline(header_stream, line) && !line.empty())
// 			{
// 				size_t colon = line.find(":");
// 				if (colon != std::string::npos) {
// 					std::string key = line.substr(0, colon);
// 					std::string value = line.substr(colon + 1);
// 					key.erase(0, key.find_first_not_of(" \t"));
// 					key.erase(key.find_last_not_of(" \t") + 1);
// 					value.erase(0, value.find_first_not_of(" \t"));
// 					value.erase(value.find_last_not_of(" \t") + 1);

// 					if (key == "Set-Cookie") {
// 						response.headers.insert(std::make_pair(key, value));
// 					} 
// 					else {
// 						response.set_header(key, value);
// 					}
// 				}
// 			}

// 			response.body = output.substr(header_end + 4);
// 		} else {
// 			// Fallback if no headers
// 			response.body = output;
// 			response.set_header("Content-Type", "text/html");
// 		}

// 		// Always set version and status
// 		response.version = "HTTP/1.1";
// 		response.set_status(200, "OK");

// 		// Set Content-Length if not already present
// 		if (response.headers.find("Content-Length") == response.headers.end())
// 			response.set_header("Content-Length", int_to_string(response.body.size()));
// 	}
// 	return 0;
// }

// In HttpResponse.hpp (or wherever you define the class)
// int handle_cgi(HttpRequest &request, HttpResponse &response, Server_block &f) {
// void set_error(int status_code, const std::string &message) {
// 	this->status_code = status_code;
// 	this->status_message = message;
// 	this->headers.clear();
// 	this->body = "<html><body><h1>" + std::to_string(status_code) + " " + message + "</h1></body></html>";
// }

// void set_success(int status_code, const std::string &message) {
// 	this->status_code = status_code;
// 	this->status_message = message;
// 	this->headers.clear();
// 	this->body = "<html><body><h1>" + std::to_string(status_code) + " " + message + "</h1></body></html>";
// }



// ==== session part ====
static SessionManager sessionManager;


std::string SessionManager::get_or_create_session_id(const std::string &client_cookie) {
	if (client_cookie.find("session_id=") != std::string::npos) {
		size_t start = client_cookie.find("session_id=") + 11;
		size_t end = client_cookie.find(";", start);
		return client_cookie.substr(start, end - start);
	}
	std::stringstream ss;
	ss << std::time(0);
	std::string new_id = ss.str();
	sessions[new_id] = 1;
	return new_id;
}

int SessionManager::get_visit_count(const std::string &session_id) {
	return sessions[session_id];
}

void SessionManager::increment_visit(const std::string &session_id) {
	sessions[session_id]++;
}



int handle_cgi(HttpRequest &request, HttpResponse &response, Server_block &f) {
	std::string script_path = "./www" + request.target;

	if (request.method == "DELETE") {
		if (access(script_path.c_str(), F_OK) == -1)
		{
			response.set_error(404, "File not found");
			return 1;
		}
		if (unlink(script_path.c_str()) == -1)
		{
			response.set_error(500, "Cannot delete file");
			return 1;
		}
		response.set_success(204, "");
		return 0;
	}
	if (request.target.empty()) {
		response.set_error(400, "Bad Request: Script path is empty");
		return 1;
	} else if (access(script_path.c_str(), F_OK) == -1) {
		response.set_error(404, "Not Found: CGI script not found");
		return 1;
	} else if (access(script_path.c_str(), X_OK) == -1) {
		response.set_error(403, "Forbidden: CGI script is not executable");
		return 1;
	}
	if (script_path.empty() || access(script_path.c_str(), X_OK) == -1)
	{
		response.set_error(404, "Invalid script");
		return 1; 
	}

	int fd_in[2], fd_out[2];
	if (pipe(fd_in) == -1 || pipe(fd_out) == -1)
	{
		response.set_error(500, "Pipe failed");
		return 1;
	}

	pid_t pid = fork();
	if (pid == -1)
	{
		response.set_error(500, "Fork failed");
		return 1;
	}

	if (pid == 0) {
		dup2(fd_in[0], STDIN_FILENO);
		dup2(fd_out[1], STDOUT_FILENO);
		close(fd_in[1]);
		close(fd_out[0]);

		// ========== Session ==========
		std::string session_id = sessionManager.get_or_create_session_id(request.cookies);
		sessionManager.increment_visit(session_id);
		int visit_count = sessionManager.get_visit_count(session_id);
		std::stringstream cookie_header;
		cookie_header << "session_id=" << session_id << "; Max-Age=600; HttpOnly";
		// response.set_header("Set-Cookie", cookie_header.str());
		
		// (void)visit_count;
		// (void)f;
		// ENV
		std::vector<std::string> env_strings;
		env_strings.push_back("VISIT_COUNT=" + int_to_string(visit_count));
		env_strings.push_back("REQUEST_METHOD=" + request.method);
		env_strings.push_back("HTTP_COOKIE=" + request.cookies);
		env_strings.push_back("QUERY_STRING=" + request.query);
		env_strings.push_back("CONTENT_LENGTH=" + int_to_string(request.body.size()));
		env_strings.push_back("SCRIPT_FILENAME=" + script_path);
		env_strings.push_back("GATEWAY_INTERFACE=CGI/1.1");
		env_strings.push_back("SERVER_PROTOCOL=HTTP/1.1");
		env_strings.push_back("REDIRECT_STATUS=200");
		env_strings.push_back("HTTP_USER_AGENT=" + request.headers["user-agent"]);


		std::vector<char*> envp;
		for (size_t i = 0; i < env_strings.size(); ++i)
			envp.push_back(strdup(env_strings[i].c_str()));
		envp.push_back(NULL);
		std::string ext = script_path.substr(script_path.find_last_of('.'));
		std::string interpreter;
		
		if (ext == ".py") interpreter = "/usr/bin/python3";
		else if (ext == ".php") interpreter = "/usr/bin/php-cgi";
		else interpreter = "";
		
		// Check if interpreter exists
		if(access(interpreter.c_str(), F_OK) == -1) {
			response.set_error(404, "Invalid script");
			return 1; //to change
		}

		char *argv[] = { NULL, NULL, NULL };
		if (!interpreter.empty()) {
			argv[0] = strdup(interpreter.c_str());
			argv[1] = strdup(script_path.c_str());
			argv[2] = NULL;
		} else {
			argv[0] = strdup(script_path.c_str());
			argv[1] = NULL;
		}

		execve(argv[0], argv, &envp[0]);
		exit(1);
	}

	close(fd_in[0]);
	close(fd_out[1]);
	if (request.method == "POST")
	{
		handle_post(request,response, f); // Assuming blocks[0] is the default server block
		write(fd_in[1], request.body.c_str(), request.body.size());
	}
	else if (request.method == "GET")
	{
		write(fd_in[1], request.body.c_str(), request.body.size());
	}
	close(fd_in[1]);

	char buffer[1024];
	std::string output;
	ssize_t n;
	while ((n = read(fd_out[0], buffer, sizeof(buffer))) > 0)
		output.append(buffer, n);
	close(fd_out[0]);
	waitpid(pid, NULL, 0);

	// == Add fallback Set-Cookie header if CGI didn't return one ==
	// if (request.cookies.find("session_id=") == std::string::npos) {
	// 	std::string session_id = sessionManager.get_or_create_session_id(request.cookies);
	// 	std::stringstream cookie_header;
	// 	cookie_header << "session_id=" << session_id << "; Max-Age=600; HttpOnly";
	// 	response.set_header("Set-Cookie", cookie_header.str());
	// 	sessionManager.increment_visit(session_id);
	// }

	if (request.cookies.find("session_id=") == std::string::npos &&
    response.headers.find("Set-Cookie") == response.headers.end()) {
	std::string session_id = sessionManager.get_or_create_session_id(request.cookies);
	std::stringstream cookie_header;
	cookie_header << "session_id=" << session_id << "; Max-Age=600; HttpOnly";
	response.set_header("Set-Cookie", cookie_header.str());
	sessionManager.increment_visit(session_id);
}

	size_t header_end = output.find("\r\n\r\n");
	if (header_end != std::string::npos) {
		std::istringstream header_stream(output.substr(0, header_end));
		std::string line;
		while (std::getline(header_stream, line)) {
			size_t colon = line.find(":");
			if (colon != std::string::npos) {
				std::string key = line.substr(0, colon);
				std::string value = line.substr(colon + 1);
				// Add this if not already present
				if (key == "Set-Cookie") {
					response.headers.insert(std::make_pair(key, value));
				}
				else
					response.set_header(key, value);
		}
}
		response.body = output.substr(header_end + 4);
	} else {
		response.body = output;
		response.set_header("Content-Type", "text/html");
	}

	response.version = "HTTP/1.1";
	response.set_status(200, "OK");
	if (response.headers.find("Content-Length") == response.headers.end())
		response.set_header("Content-Length", int_to_string(response.body.size()));

	return 0;
}
