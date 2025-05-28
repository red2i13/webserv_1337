#include <iostream>
#include <unistd.h>
#include <cstring>
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    int reuse = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, 1) < 0) {
        perror("listen");
        return 1;
    }

    std::cout << "Server listening on port 8080...\n";

    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("accept");
        return 1;
    }

    std::string request;
    char buffer[4096];
    ssize_t bytes;

    while ((bytes = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
        request.append(buffer, bytes);
        if (request.find("\r\n\r\n") != std::string::npos)
            break;
    }

    std::cout << "Received request:\n" << request << std::endl;

    close(client_fd);
    close(server_fd);
    return 0;
}
