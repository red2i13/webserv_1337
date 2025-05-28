#include <miniserv.hpp>

//test use bind socket listen accept

//create mini server listen to port 80 in local ip 127.0.0.1 if the client connect to this server should be print message
int main()
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == -1)
        write_error_message(errno);
    return 0;
}