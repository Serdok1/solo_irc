#include "Server.hpp"

Server::Server(int port_number, std::string server_password): _port_number(port_number), _server_password(server_password)
{
    std::cout << "Server created successfully!" << std::endl;
}

Server::~Server(void)
{
    std::cout << "Server destructed succsefully" << std::endl;
}