#include "IRC.hpp"
#include "Server.hpp"

void initServer(Server *server)
{
    if (server->createSocket() < 0)
    {
        perror("socket create error");
        exit(EXIT_FAILURE);
    }

    if (server->setSocketOpt() < 0)
    {
        perror("sockopt error");
        exit(EXIT_FAILURE);
    }

    if (server->setNonBlock() < 0)
    {
        perror("fcntl error");
        exit(EXIT_FAILURE);
    }

    server->setAddressProperties(AF_INET, INADDR_ANY);

    if (server->bindSocket() < 0)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
    }

    if (server->listenSocket() < 0)
    {
        perror("listen error");
        exit(EXIT_FAILURE);
    }
}

bool clientCanAuthenticate(Client &client)
{
    return !client.getIsAuth() && !client.getPassword().empty() && !client.getUsername().empty() && !client.getNickname().empty();
}

void authenticateClient(Client &client)
{
    client.setIsAuth(true);
    std::string message = LOGIN(client.getNickname(), client.getUsername());
    send(client.getClientFd(), message.c_str(), message.length(), 0);
}

void acceptSetClient(Server *server, fd_set &current_sockets, int &max_socket)
{
    if (server->acceptSocket() < 0)
    {
        perror("accept error");
        exit(EXIT_FAILURE);
    }
    else
    {
        FD_SET(server->getNewSocket(), &current_sockets);
        if (server->getNewSocket() > max_socket)
        {
            max_socket = server->getNewSocket();
        }
        Client client = Client(server->getNewSocket());
        server->addNewClient(client);
        std::cout << "selectServer: new connection from " << inet_ntoa(server->getSocketAddress().sin_addr) << "on socket " << server->getNewSocket() << std::endl;
    }
}