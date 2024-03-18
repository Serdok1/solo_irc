#include "Server.hpp"

Server::Server(int port_number, std::string server_password): _port_number(port_number), _server_password(server_password)
{
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
    this->_sockaddr.sin_addr.s_addr = address;
    this->_sockaddr.sin_port = htons(this->_port_number); 
}

int Server::bindSocket(void)
{
    if(bind(this->_sockfd, (struct sockaddr *)&_sockaddr, _sockaddr_len) < 0)
        return -1;
    return 0;
}

int Server::listenSocket(void)
{
    if(listen(this->_sockfd, 10))
        return -1;
    return 0;
}

int Server::acceptSocket(void)
{
    if((this->_new_socket = accept(this->_sockfd, (struct sockaddr *)&this->_sockaddr, &this->_sockaddr_len)) < 0)
        return -1;
    return 0;
}