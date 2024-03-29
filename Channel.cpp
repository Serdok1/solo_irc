#include "Channel.hpp"

Channel::Channel(){}

Channel::Channel(std::string name, std::string password): _name(name), _password(password){}

Channel::Channel(std::string name, std::string password, Client &client): _name(name), _password(password){
    this->channelClients.push_back(client);
}

Channel::~Channel(){}

std::string Channel::getName(void)
{
    return(this->_name);
}

std::string Channel::getPassword(void)
{
    return(this->_password);
}

std::string Channel::getTopic() {
    return (_topic);
}

std::vector<Client> Channel::getOperatorArray() {
    return (this->_operator_array);
}

void Channel::setName(std::string name)
{
    this->_name = name;
}

void Channel::setPassword(std::string password)
{
    this->_password = password;
}

void Channel::setTopic(std::string topic) {
    this->_topic = topic;
}


void Channel::addOperator(Client &client) {
    this->_operator_array.push_back(client);
}


