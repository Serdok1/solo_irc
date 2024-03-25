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

int Server::getSocketFd(void){
    return this->_sockfd;
}

std::vector<Client> Server::getClientArray(void){
    return this->_client_array;
}
std::vector<Channel> Server::getChannelArray(void){
    return this->_channel_array;
}

std::string Server::getHostname(void){
    return this->_hostname;
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

void Server::setSocketFd(int sockfd){
    this->_sockfd = sockfd;
}

void Server::setClientArray(std::vector<Client> clientArray){
    this->_client_array = clientArray;
}

void Server::setChannelArray(std::vector<Channel> channelArray){
    this->_channel_array = channelArray;
}

void Server::setHostname(std::string hostname){
    this->_hostname = hostname;
}