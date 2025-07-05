#include "run_cgi.hpp"

// std::string int_to_string(int n) {
//     std::stringstream ss;
//     ss << n;
//     return ss.str();
// }

// int handle_cgi(const Request &request, Response &response) {
//     int fd_in[2], fd_out[2];
//     if (pipe(fd_in) == -1 || pipe(fd_out) == -1) {
//         strerror(errno);
//         return 1;
//     }

//     pid_t pid = fork();
//     if (pid == -1) {
//         strerror(errno);
//         return 1;
//     }

//     if (pid == 0) {
//         // --- CHILD ---
//         dup2(fd_in[0], STDIN_FILENO);
//         dup2(fd_out[1], STDOUT_FILENO);

//         close(fd_in[1]);
//         close(fd_out[0]);

//         // --- Build environment variables ---
//         std::vector<std::string> env_strings;
//         env_strings.push_back("REQUEST_METHOD=" + request.method);
//         env_strings.push_back("QUERY_STRING=" + request.query);
//         env_strings.push_back("CONTENT_LENGTH=" + int_to_string(request.body.size()));
//         std::map<std::string, std::string>::const_iterator it = request.headers.find("Content-Type");
//         if (it != request.headers.end())
//             env_strings.push_back("CONTENT_TYPE=" + it->second);
//         env_strings.push_back("GATEWAY_INTERFACE=CGI/1.1");
//         env_strings.push_back("SCRIPT_NAME=" + request.script_path);
//         env_strings.push_back("SERVER_PROTOCOL=HTTP/1.1");

//         // Convert to char*[]
//         std::vector<char*> envp;
//         for (size_t i = 0; i < env_strings.size(); ++i)
//             envp.push_back(strdup(env_strings[i].c_str()));
//         envp.push_back(NULL);

//         char *argv[] = { strdup(request.script_path.c_str()), NULL };

//         execve(argv[0], argv, &envp[0]);
//         strerror(errno);
//         exit(1);
//     } else {
//         // --- PARENT ---
//         close(fd_in[0]);
//         close(fd_out[1]);

//         if (request.method == "POST") {
//             write(fd_in[1], request.body.c_str(), request.body.size());
//         }
//         close(fd_in[1]);

//         char buffer[1024];
//         std::string output;
//         ssize_t n;
//         while ((n = read(fd_out[0], buffer, sizeof(buffer))) > 0) {
//             output.append(buffer, n);
//         }
//         close(fd_out[0]);

//         response.raw = output;
//         waitpid(pid, NULL, 0);
//     }
//     return 0;
// }

// int main() {
//     Request request;

//     // Set up a basic GET request
//     request.method = "GET";
//     request.query = "name=70rn4d0";  // Will become QUERY_STRING
//     request.body = "";
//     request.script_path = "./cgi-bin/hello.py";  // Adjust path as needed
//     request.headers["Content-Type"] = "application/x-www-form-urlencoded";

//     Response response;

//     // Run CGI
//     int result = handle_cgi(request, response);

//     // Output
//     if (result == 0) {
//         std::cout << "=== CGI Response ===\n";
//         std::cout << response.raw << std::endl;
//     } else {
//         std::cerr << "CGI execution failed.\n";
//     }

//     return 0;
// }

// ==== Helper ====
std::string int_to_string(int n) {
	std::stringstream ss;
	ss << n;
	return ss.str();
}

// ==== Main CGI Handler ====
int handle_cgi(const Request &request, Response &response) {
	
	//CHECK PATH
	if (request.script_path.empty()) {
		write(2, "Script path is empty\n", 22);
		return 1;
	}
	if (access(request.script_path.c_str(), F_OK) == -1) {
		write(2, "Script not found\n", 17);
		return 1;
	}
	if (access(request.script_path.c_str(), X_OK) == -1) {
		write(2, "Script is not executable\n", 25);
		return 1;
	}
	int fd_in[2], fd_out[2];

	if (pipe(fd_in) == -1 || pipe(fd_out) == -1) {
		write(2, "pipe\n", 5);
		return 1;
	}

	pid_t pid = fork();
	if (pid == -1) {
		write(2,"fork\n", 5);
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
		env_strings.push_back("SCRIPT_NAME=" + request.script_path);
		env_strings.push_back("SERVER_PROTOCOL=HTTP/1.1");
		env_strings.push_back("REDIRECT_STATUS=200");

		std::vector<char*> envp;
		for (size_t i = 0; i < env_strings.size(); ++i)
			envp.push_back(strdup(env_strings[i].c_str()));
		envp.push_back(NULL);

		char *argv[] = { strdup(request.script_path.c_str()), NULL };

		execve(argv[0], argv, &envp[0]);

		// If execve fails
		write(2, "execve failed\n", 15);
		exit(1);
	} else {
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
			output.append(buffer, n);
		close(fd_out[0]);

		waitpid(pid, NULL, 0);

		// Split headers and body
		size_t header_end = output.find("\r\n\r\n");
		if (header_end != std::string::npos) {
			response.headers = output.substr(0, header_end);
			response.body = output.substr(header_end + 4);
		} else {
			response.body = output;
		}
		response.raw = output;
	}

	return 0;
}

// ==== MAIN ====
int main() {
	Request request;
	request.method = "GET";
	request.query = "name=70rn4d0";
	request.body = "";
	request.script_path = "./cgi-bin/hello.py";
	request.headers["Content-Type"] = "application/x-www-form-urlencoded";

	Response response;

	int result = handle_cgi(request, response);

	if (result == 0) {
		std::cout << "=== CGI HEADERS ===\n" << response.headers << "\n";
		std::cout << "=== CGI BODY ===\n" << response.body << "\n";
	} else {
		std::cerr << "❌ CGI execution failed\n";
	}
	return 0;
}
