#include "../includes/run_cgi.hpp"


// ==== Helper ====
std::string int_to_string(int n) {
	std::stringstream ss;
	ss << n;
	return ss.str();
}

// ==== Main CGI Handler ====
int handle_cgi(HttpRequest &request, HttpResponse &response) {
	std::string script_path = "./www" + request.target;
	// CHECK METHID
	if (request.method == "DELETE") 
	{
		if (access(script_path.c_str(), F_OK) == -1) {
			response.set_status(404, "Not Found");
			response.set_header("Content-Type", "text/html");
			response.set_body("<html><body><h1>404 Not Found</h1><p>File to delete not found.</p></body></html>");
			return 1;
		}
		if (unlink(script_path.c_str()) == -1) {
			response.set_status(500, "Internal Server Error");
			response.set_header("Content-Type", "text/html");
			response.set_body("<html><body><h1>500 Internal Server Error</h1><p>Cannot delete file.</p></body></html>");
			return 1;
		}
		response.set_status(204, "No Content");
		response.set_header("Content-Type", "text/html");
		response.set_body(""); // No body for 204
		return 0;
	}
	// CHECK PATH
	if (request.target.empty()) {
		response.set_status(400, "Bad Request");
		response.set_header("Content-Type", "text/html");
		response.set_body("<html><body><h1>400 Bad Request</h1><p>Script path is empty.</p></body></html>");
		return 1;
	}
	else if (access(script_path.c_str(), F_OK) == -1) {
		response.set_status(404, "Not Found");
		response.set_header("Content-Type", "text/html");
		response.set_body("<html><body><h1>404 Not Found</h1><p>CGI script not found.</p></body></html>");
		return 1;
	}
	else if (access(script_path.c_str(), X_OK) == -1) {
		response.set_status(403, "Forbidden");
		response.set_header("Content-Type", "text/html");
		response.set_body("<html><body><h1>403 Forbidden</h1><p>CGI script is not executable.</p></body></html>");
		return 1;
	}
	int fd_in[2], fd_out[2];
	if (pipe(fd_in) == -1 || pipe(fd_out) == -1) {
		write(2, "pipe\n", 5);
		return 1;
	}
	
	pid_t pid = fork();
	if (pid == -1) {
		write(2, "fork\n", 5);
		return 1;
	}
	
	if (pid == 0) {
		// CHILD PROCESS
		dup2(fd_in[0], STDIN_FILENO);
		dup2(fd_out[1], STDOUT_FILENO);
		close(fd_in[1]);
		close(fd_out[0]);
		
		// Build ENV
		std::vector<std::string> env_strings;
		env_strings.push_back("REQUEST_METHOD=" + request.method);
		env_strings.push_back("QUERY_STRING=" + request.query);
		env_strings.push_back("CONTENT_LENGTH=" + int_to_string(request.body.size()));
		std::map<std::string, std::string>::const_iterator it = request.headers.find("Content-Type");
		if (it != request.headers.end())
		env_strings.push_back("CONTENT_TYPE=" + it->second);
		env_strings.push_back("GATEWAY_INTERFACE=CGI/1.1");
		env_strings.push_back("SCRIPT_NAME=" + request.target);
		env_strings.push_back("SERVER_PROTOCOL=HTTP/1.1");
		env_strings.push_back("REDIRECT_STATUS=200");
		
		env_strings.push_back("SCRIPT_FILENAME=" + script_path);
		env_strings.push_back("PATH_INFO=" + request.target);
		env_strings.push_back("REQUEST_URI=" + request.target);
		
		
		std::vector<char*> envp;
		for (size_t i = 0; i < env_strings.size(); ++i)
		envp.push_back(strdup(env_strings[i].c_str()));
		envp.push_back(NULL);
		
		std::string ext = script_path.substr(script_path.find_last_of('.'));
		std::string interpreter;
		
		if (ext == ".py") interpreter = "/usr/bin/python3";
		else if (ext == ".php") interpreter = "/usr/bin/php-cgi";
		else interpreter = "";
		if(access(interpreter.c_str(), F_OK) == -1) {
			std::cout << "med zaml" << std::endl;			
			exit (5); // No interpreter found, will try to exec script directly
		}
		char *argv[] = { NULL, NULL, NULL };
		if (!interpreter.empty()) {
			argv[0] = strdup(interpreter.c_str());
			argv[1] = strdup(script_path.c_str());
			argv[2] = NULL;
			execve(argv[0], argv, &envp[0]);
		} else {
			argv[0] = strdup(script_path.c_str());
			argv[1] = NULL;
			execve(argv[0], argv, &envp[0]);
		}
		perror("execve failed");
		
		// Free memory only if execve fails
		for (size_t i = 0; i < envp.size() - 1; ++i)
		free(envp[i]);
		
		for (int i = 0; argv[i]; ++i)
		free(argv[i]);
		
		exit(1);
	} 
	else {
		// PARENT PROCESS
		close(fd_in[0]);
		close(fd_out[1]);
		
		if (request.method == "POST")
		write(fd_in[1], request.body.c_str(), request.body.size());
		close(fd_in[1]);
		
		char buffer[1024];
		std::string output;
		ssize_t n;
		while ((n = read(fd_out[0], buffer, sizeof(buffer))) > 0)
		{
			std::cout << "infinit loop" << std::endl;	
			output.append(buffer, n);
		}
		std::cout << response.status_code << response.status_message << std::endl;
		std::cout << "i am here" << std::endl;
		close(fd_out[0]);
		std::cerr << "Before waitpid" << std::endl;
		waitpid(pid, NULL, 0);
		std::cerr << "After waitpid";
		// Split headers and body
		size_t header_end = output.find("\r\n\r\n");
		if (header_end != std::string::npos) {
			std::string header_block = output.substr(0, header_end);
			std::istringstream header_stream(header_block);
			std::string line;

			while (std::getline(header_stream, line) && !line.empty()) {
				size_t colon = line.find(":");
				if (colon != std::string::npos) {
					std::string key = line.substr(0, colon);
					std::string value = line.substr(colon + 1);
					key.erase(0, key.find_first_not_of(" \t"));
					key.erase(key.find_last_not_of(" \t") + 1);
					value.erase(0, value.find_first_not_of(" \t"));
					value.erase(value.find_last_not_of(" \t") + 1);
					response.set_header(key, value);
				}
			}

			response.body = output.substr(header_end + 4);
		} else {
			// Fallback if no headers
			response.body = output;
			response.set_header("Content-Type", "text/html");
		}

		// Always set version and status
		response.version = "HTTP/1.1";
		response.set_status(200, "OK");

		// Set Content-Length if not already present
		if (response.headers.find("Content-Length") == response.headers.end())
			response.set_header("Content-Length", int_to_string(response.body.size()));
	}
	return 0;
}
