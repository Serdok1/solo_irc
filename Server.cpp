#include "Server.hpp"

Server::Server(int port_number, std::string server_password): _port_number(port_number), _server_password(server_password)
{
    if(port_number > 65535 || port_number < 1024)
    {
        std::cout << "Please select port between 1024 and 65535" << std::endl;
        exit(EXIT_FAILURE);
    }
    this->_sockaddr_len = sizeof(this->_sockaddr);
    std::cout << "Server created successfully!" << std::endl;
}

Server::~Server(void)
{
    std::cout << "Server destructed succsefully" << std::endl;
}

int Server::createSocket(void)
{
    if((this->_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;
    return 0;
}

int Server::setSocketOpt(void){
    int opt = 1;
    if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        return -1;
    return 0;
}

int Server::setNonBlock(void){
    if(fcntl(this->_sockfd, F_SETFL, O_NONBLOCK) < 0)
        return -1;
    return 0;
}

void Server::setAddressProperties(sa_family_t family, in_addr_t address)
{
    this->_sockaddr.sin_family = family;
    this->_sockaddr.sin_addr.s_addr = htonl(address);
    this->_sockaddr.sin_port = htons(this->_port_number); 
}

int Server::bindSocket(void)
{
    if(bind(this->_sockfd, (struct sockaddr *)&this->_sockaddr, this->_sockaddr_len) < 0)
        return -1;
    return 0;
}

int Server::listenSocket(void)
{
    if(listen(this->_sockfd, this->_sockfd))
        return -1;
    return 0;
}

int Server::acceptSocket(void)
{
    if((this->_new_socket = accept(this->_sockfd, (struct sockaddr *)&this->_sockaddr, &this->_sockaddr_len)) < 0)
        return -1;
    return 0;
}

void Server::addNewClient(Client &client)
{
    this->_client_array.push_back(client);
}

void Server::deleteClient(int socket_fd)
{
    for(int i = 0; i < (int)this->_client_array.size(); i++)
    {
        if(this->_client_array[i].getClientFd() == socket_fd)
        {
            this->_client_array.erase(this->_client_array.begin() + i);
            break;
        }
    }
}

Client *Server::findClient(int socket_fd)
{
    for(int i = 0; i < (int)this->_client_array.size(); i++)
    {
        if(this->_client_array[i].getClientFd() == socket_fd)
        {
            return &this->_client_array[i];
        }
    }
    return NULL;
}