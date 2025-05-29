#include "miniserv.hpp"

//test use bind socket listen accept

//create mini server listen to port 80 in local ip 127.0.0.1 if the client connect to this server should be print message
int main()
{
    int check = 0;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == -1)
        write_error_message(errno);
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    check = bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if(check == -1)
        write_error_message(errno);
    
    check = listen(serverSocket, 5);
    if(check == -1)
        write_error_message(errno);
    
    std::cout << "The server listen on port 8080: " << std::endl;
    //the server part is ready//

    int clientSocket = accept(serverSocket, nullptr, nullptr);
    

    return 0;
}