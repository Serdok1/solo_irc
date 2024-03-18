#include "IRC.hpp"
#include "Server.hpp"

int main(int ac, char **av){
    if(ac != 3){
        std::cout << "Wrong arguments! You have to run the server with <port_number> and <server_password>." << std::endl;
        return 1;
    }
    Server server(atoi(av[1]), av[2]);
    //Server socket things...
    //Create socket
    if(server.createSocket() < 0)
    {
        perror("socket creat error");
        exit(EXIT_FAILURE);
    }
    
    //Set option to socket
    if(server.setSocketOpt() < 0)
    {
        perror("sockopt error");
        exit(EXIT_FAILURE);
    }
    //Set socket to non blocking mode
    if(server.setNonBlock() < 0)
    {
        perror("fcntl error");
        exit(EXIT_FAILURE);
    }
    //Set address properties
    server.setAddressProperties(AF_INET, INADDR_ANY);
    //Bind the socket in address and port that specified in addr
    if(server.bindSocket() < 0)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
    }
    //Listen socket
    if(server.listenSocket() < 0)
    {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        if(server.acceptSocket() < 0)
        {
            perror("accept error");
            exit(EXIT_FAILURE);
        }
        char buffer[1028] = {0};
        size_t read_value;
        if((read_value = recv(server.getNewSocket(), buffer, sizeof(buffer) - 1, 0)) < 0)
        {
            perror("recv error");
            exit(EXIT_FAILURE);
        }
        buffer[read_value] = '\0';
        std::cout << "----------\n" << buffer << "----------" << std::endl;
    }
}