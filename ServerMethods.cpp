#include "Server.hpp"

std::string Server::getServerPassword(void){
    return this->_server_password;
}

int Server::getPortNumber(void){
    return this->_port_number;
}

void Server::setServerPassword(std::string server_password){
    this->_server_password = server_password;
}

void Server::setPortNumber(int port_number){
    this->_port_number = port_number;
}