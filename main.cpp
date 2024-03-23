#include "IRC.hpp"
#include "Server.hpp"

void acceptSetClient(Server *server, fd_set *current_sockets, int *max_socket){
    if(server->acceptSocket() < 0)
    {
        perror("accept error");
        exit(EXIT_FAILURE);
    }
    FD_SET(server->getNewSocket(), current_sockets);
    if(server->getNewSocket() > *max_socket){
        *max_socket = server->getNewSocket();
    }
    Client client = Client(server->getNewSocket());
    server->addNewClient(client);
    printf("selectServer: new connection from %s on "
            "socket %d\n", inet_ntoa(server->getSocketAddress().sin_addr), server->getNewSocket());
}

void handleClient(Server *server, int *i, fd_set *current_sockets, int *max_socket){
    char buffer[1028] = {0};
    size_t read_value;
    if((read_value = recv(*i, buffer, sizeof(buffer) - 1, 0)) <= 0)
    {
        if(read_value == 0){
            std::cout << "client left the server, socket no: " << i << std::endl;
            close(*i);
            server->deleteClient(*i);
            FD_CLR(*i, current_sockets);
            (*max_socket)--;
        }
        else{
            perror("recv error");
            close(*i);
            server->deleteClient(*i);
            FD_CLR(*i, current_sockets);
            (*max_socket)--;
            exit(EXIT_FAILURE);
        }
    }else{
        buffer[read_value] = '\0';
        std::cout << "----------\n" << buffer << "----------" << std::endl;
        std::istringstream iss(buffer);
        std::string token, user, pass, nick;
        Client &tempClient = *server->findClient(*i);
        while (iss >> token) {
            if (token == "NICK"){
                iss >> nick;
                tempClient.setNickname(nick);
            }
            else if (token == "PASS")
                iss >> pass;
            else if (token == "USER"){
                iss >> user;
                tempClient.setUsername(user);
            }
        }
    }
}

int main(int ac, char **av){
    if(ac != 3){
        std::cout << "Wrong arguments! You have to run the server with <port_number> and <server_password>." << std::endl;
        return 1;
    }
    Server *server = new Server(atoi(av[1]), av[2]);

    if(server->createSocket() < 0)
    {
        perror("socket create error");
        exit(EXIT_FAILURE);
    }
    
    if(server->setSocketOpt() < 0)
    {
        perror("sockopt error");
        exit(EXIT_FAILURE);
    }

    if(server->setNonBlock() < 0)
    {
        perror("fcntl error");
        exit(EXIT_FAILURE);
    }

    server->setAddressProperties(AF_INET, INADDR_ANY);

    if(server->bindSocket() < 0)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    if(server->listenSocket() < 0)
    {
        perror("listen error");
        exit(EXIT_FAILURE);
    }


    fd_set current_sockets, ready_sockets;
    FD_ZERO(&current_sockets);
    FD_SET(server->getSocketFd(), &current_sockets);
    int max_socket = server->getSocketFd();
    std::string user,nick,pass;
    while(1)
    {
        //because select is destructive
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
                    acceptSetClient(server, &current_sockets, &max_socket);
                    std::cout << "max_socket: " << max_socket << std::endl;
                }
                else
                {
                    handleClient(server, &i, &current_sockets, &max_socket);
                }
            }
        }
        std::cout << (int)server->getClientArray().size() << std::endl;
        for(int i = 0; i < (int)server->getClientArray().size(); i++)
        {
            std::cout<< "Username: " << server->getClientArray()[i].getUsername()\
            << " Nickname: " << server->getClientArray()[i].getNickname() << " socket_fd: "\
            << server->getClientArray()[i].getClientFd() << std::endl;
        }
    }
}