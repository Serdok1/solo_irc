#include "Client.hpp"

Client::Client(){}

Client::Client(int client_fd): _client_fd(client_fd){}

Client::~Client(){}

int Client::getClientFd() {
    return(this->_client_fd);
}

std::string Client::getNickname() {
    return (this->_nickname);
}

std::string Client::getUsername() {
    return (this->_username);
}

std::string Client::getRealName() {
    return (this->_real_name);
}

std::string Client::getHostname() {
    return (this->_hostname);
}

void Client::setClientFd(int socket_fd) {
    this->_client_fd = socket_fd;
}

void Client::setNickname(std::string nickname) {
    this->_nickname = nickname;
}

void Client::setUsername(std::string username) {
    this->_username = username;
}

void Client::setRealName(std::string realName) {
    this->_real_name = realName;
}

void Client::setHostname(std::string hostname) {
    this->_hostname = hostname;
}

