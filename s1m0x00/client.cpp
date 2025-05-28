#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);  // Connect to port 8080
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // localhost

    if (connect(sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    std::string request = 
        "GET /hello?name=test HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: MiniClient/1.0\r\n"
        "Accept: */*\r\n"
        "\r\n";

    send(sock, request.c_str(), request.size(), 0);

    close(sock);
    return 0;
}
