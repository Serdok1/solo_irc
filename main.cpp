#include "IRC.hpp"
#include "Server.hpp"

int main(int ac, char **av){
    if(ac != 3){
        std::cout << "Wrong arguments! You have to run the server with <port_number> and <server_password>." << std::endl;
        return 1;
    }
    Server *server = new Server(atoi(av[1]), av[2]);

    initServer(server);

    fd_set current_sockets, ready_sockets;
    FD_ZERO(&current_sockets);
    FD_ZERO(&ready_sockets);
    FD_SET(server->getSocketFd(), &current_sockets);
    int max_socket = server->getSocketFd();
    while(1)
    {
        ready_sockets = current_sockets;

        if(select(max_socket + 1, &ready_sockets, NULL, NULL, NULL) < 0){
            perror("select error");
            exit(EXIT_FAILURE);
        }

        for(int i = 0; i <= max_socket; i++)
        {
            if(FD_ISSET(i, &ready_sockets))
            {
                if(i == server->getSocketFd())
                {
                    acceptSetClient(server, current_sockets, max_socket);
                }
                else
                {
                    handleClient(server, i, current_sockets);
                }
            }
        }
        for(int i = 0; i < (int)server->getChannelArray().size(); i++)
        {
            for(int j = 0; j < (int)server->getChannelArray()[i].getOperatorArray().size(); j++)
            {
                std::cout << server->getChannelArray()[i].getName() << " operator is: " << server->getChannelArray()[i].getOperatorArray()[j].getNickname() << std::endl;
            }
        }
    }
}