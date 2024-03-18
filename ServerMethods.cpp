#include "Server.hpp"

std::string Server::getServerPassword(void){
    return this->_server_password;
}

int Server::getPortNumber(void){
    return this->_port_number;
}

struct sockaddr_in Server::getSocketAddress(void){
    return this->_sockaddr;
}

socklen_t Server::getSocketAddressLength(void){
    return this->_sockaddr_len;
}

int Server::getNewSocket(void){
    return this->_new_socket;
}

void Server::setServerPassword(std::string server_password){
    this->_server_password = server_password;
}

void Server::setPortNumber(int port_number){
    this->_port_number = port_number;
}

void Server::setSocketAddress(struct sockaddr_in sockaddr){
    this->_sockaddr = sockaddr;
}

void Server::setSocketAddressLength(socklen_t sockaddr_len){
    this->_sockaddr_len = sockaddr_len;
}

void Server::setNewSocket(int new_socket){
    this->_new_socket = new_socket;
}