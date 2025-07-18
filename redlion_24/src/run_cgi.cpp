#include "../includes/run_cgi.hpp"

// ==== session part ====
static SessionManager sessionManager;

std::string SessionManager::get_or_create_session_id(const std::string &client_cookie) {
	if (client_cookie.find("session_id=") != std::string::npos) {
		size_t start = client_cookie.find("session_id=") + 11;
		size_t end = client_cookie.find(";", start);
		std::string sss = client_cookie.substr(start, end - start);
		if (sessions.count(sss))
			return sss;
		else{

			sessions[sss] = 1;
			std::cout << "New session created: " << sss << std::endl;
		}
		return sss;
	}
	std::stringstream ss;
	ss << std::time(0);
	std::string new_id = ss.str();
	sessions[new_id] = 0;
	return new_id;
}

int SessionManager::track(const std::string &session_id) {
	std::map<std::string, int>::iterator it = sessions.find(session_id);
	if (it == sessions.end()) {
		sessions[session_id] = 1;
	} else {
		sessions[session_id] = it->second + 1;
	}
	return sessions[session_id];
}

bool isDirectory(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

int check_path(HttpRequest &request, HttpResponse &response, Server_block &f) {

	Location loc = f.get_location_block(request.target);
	std::string script_path = loc.path + request.target;
	
	if (isDirectory(script_path)) {
		response.set_error(403, "X Forbidden: CGI script is a directory");
		return 1; 
	}
	if (request.target.empty()) {
		response.set_error(400, "Bad Request: Script path is empty");
		return 1;
	} 
	else if (access(script_path.c_str(), F_OK) == -1) {
		response.set_error(404, "Not Found: CGI script not found");
		return 1;
	} 
	else if (access(script_path.c_str(), X_OK) == -1) {
		response.set_error(403, "Forbidden: CGI script is not executable");
		return 1;
	}
	if (script_path.empty() || access(script_path.c_str(), X_OK) == -1)
	{
		response.set_error(404, "Invalid script");
		return 1; 
	}
	return 0;
}

int SessionManager::track_and_get_visit_count(const std::string &session_id)
{
	(void)track(session_id); // Track the session
	return sessionManager.sessions.size();
}

int handle_cgi(HttpRequest &request, HttpResponse &response, Server_block &f) 
{	
	Location loc = f.get_location_block(request.target);
	std::string script_path = loc.path + request.target;

	//check meth
	if (request.method == "DELETE") {
		response.set_error(405, "Method Not Allowed");
		return 1;
	}
	//path check
	if (check_path(request, response, f) != 0) {
		return 1;
	}

	int fd_in[2], fd_out[2];
	if (pipe(fd_in) == -1 || pipe(fd_out) == -1)
	{
		response.set_error(500, "Pipe failed");
		return 1;
	}

	std::string session_id = sessionManager.get_or_create_session_id(request.cookies);
	std::cout << session_id << std::endl;
	int visit_count = sessionManager.track_and_get_visit_count(session_id);
	
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
		
		std::stringstream cookie_header;
		cookie_header << "session_id=" << session_id << "; Max-Age=600; HttpOnly";

		// ENV
		std::vector<std::string> env_strings;
		env_strings.push_back("REQUEST_METHOD=" + request.method);
		env_strings.push_back("HTTP_COOKIE=" + request.cookies);
		env_strings.push_back("visit_count=" + int_to_string(visit_count));
		std::cout << "visit_count: " << visit_count << std::endl;
		env_strings.push_back("QUERY_STRING=" + request.query);
		env_strings.push_back("CONTENT_LENGTH=" + int_to_string(request.body.size()));
		env_strings.push_back("GATEWAY_INTERFACE=CGI/1.1");
		env_strings.push_back("SERVER_PROTOCOL=HTTP/1.1");
		env_strings.push_back("REDIRECT_STATUS=200");
		env_strings.push_back("HTTP_USER_AGENT=" + request.headers["user-agent"]);
		env_strings.push_back("SCRIPT_NAME=" + script_path);

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
		Location loc = f.get_location_block(request.target);
		handle_post(request,response, f, loc.upload_path);
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


	if (request.cookies.find("session_id=") == std::string::npos &&
    response.headers.find("Set-Cookie") == response.headers.end()) {
	std::string session_id = sessionManager.get_or_create_session_id(request.cookies);
	std::stringstream cookie_header;
	cookie_header << "session_id=" << session_id << "; Max-Age=600";
	response.set_header("Set-Cookie", cookie_header.str());
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
