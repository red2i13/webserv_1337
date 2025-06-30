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
        "POST /upload HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: MiniClient/1.0\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
        "zz\r\n"         // Invalid chunk size (should be hex)
        "Hello World\r\n"
        "0\r\n"
        "\r\n";

    // std::string body = "TooShortData"; // 12 bytes
    // std::string headers =
    //     "POST /upload HTTP/1.1\r\n"
    //     "Host: localhost:8080\r\n"
    //     "User-Agent: MiniClient/1.0\r\n"
    //     "Content-Type: text/plain\r\n"
    //     "Content-Length: 20\r\n"   // Declared length is 20, but we're sending only 12 bytes
    //     "\r\n";

    // std::string request = headers + body;

    send(sock, request.c_str(), request.size(), 0);

    char buffer[1024];
    ssize_t bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        std::cout << "Server response:\n" << buffer << std::endl;
    }

    close(sock);
    return 0;
}