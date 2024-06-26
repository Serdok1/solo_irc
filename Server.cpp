#include "Server.hpp"

Server::Server(int port_number, std::string server_password): _port_number(port_number), _server_password(server_password)
{
    if(port_number > 65535 || port_number < 1024)
    {
        std::cout << "Please select port between 1024 and 65535" << std::endl;
        exit(EXIT_FAILURE);
    }
    this->_sockaddr_len = sizeof(this->_sockaddr);
    char str_h[256];
    gethostname(str_h, sizeof(str_h));
    std::string _h(str_h);
    setHostname(_h);
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
    if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
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
    memset(&(this->_sockaddr.sin_zero), '\0', 8);
}

int Server::bindSocket(void)
{
    if(bind(this->_sockfd, (struct sockaddr *)&this->_sockaddr, this->_sockaddr_len) < 0)
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

Client *Server::findClient_WFD(int socket_fd)
{
    for(int i = 0; i < (int)this->_client_array.size(); i++)
    {
        if(this->_client_array[i].getClientFd() == socket_fd)
        {
            return &this->_client_array[i];
        }
    }
    return nullptr;
}

Client *Server::findClient_WNAME(std::string name)
{
    for(int i = 0; i < (int)this->_client_array.size(); i++)
    {
        if(this->_client_array[i].getNickname() == name)
        {
            return &this->_client_array[i];
        }
    }
    return nullptr;
}

void Server::joinCommand(Client &client, std::string channelName, std::string channelPassword) {
    std::string mess;
    Channel &tempChannel = *getFindChannel(channelName);
    if(&tempChannel == nullptr)
    {
        _channel_array.push_back(Channel(channelName, channelPassword, client));
        mess = "JOIN You are now in channel "  + channelName +" \r\n";
        std::string buffer = ":" + client.getNickname() + "!" + client.getUsername() + "@" + getHostname() + ": "  +  mess + "\r\n";;
        send(client.getClientFd(), buffer.c_str(), buffer.length(), 0);
    }
    else
    {
        if(checkChannel(client, tempChannel))
        {
            mess = USERONCHANNEL(client.getNickname(), tempChannel.getName());
            send(client.getClientFd(), mess.c_str(), mess.length(), 0);
        }
        else
        {
            if((!tempChannel.getPassword().compare(channelPassword)))
            { 
                tempChannel.channelClients.push_back(client);
                mess = "JOIN You are now in channel "  + channelName +" \r\n";
                std::string buffer = ":" + client.getNickname() + "!" + client.getUsername() + "@" + getHostname() + ": "  +  mess + "\r\n";;
                send(client.getClientFd(), buffer.c_str(), buffer.length(), 0);
            }
            else
            {
                mess = BADCHANNEL(channelName);
                send(client.getClientFd(), mess.c_str(), mess.length(), 0);
            }
        }
    }
}

void Server::msgCommand(Client &client, std::string send_to, std::string buffer){
    if (send_to[0] == '#') 
    {
        Channel &tempChannel = *getFindChannel(send_to);
        if(&tempChannel != nullptr)
        {
            std::string identity = ":" + client.getNickname() + "!" + client.getUsername() + "@" + getHostname() + ": ";
	        std::string message = identity + buffer + "\r\n";
            for (int i = 0; i < (int)tempChannel.channelClients.size(); i++)
            {
                if (checkChannel(client, tempChannel)) {
                    if (tempChannel.channelClients[i].getClientFd() != client.getClientFd() && tempChannel.channelClients[i].getClientFd() != 0)
                        send(tempChannel.channelClients[i].getClientFd(), message.c_str(), message.length(), 0);
                }
            }
        }
        else {
            std::string message = NO_SUCH_CHANNEL(client.getNickname(), send_to);
            send(client.getClientFd(), message.c_str(), message.length(), 0);
        }
    }
    else {
        Client &tempClient = *findClient_WNAME(send_to);
        if (&tempClient != nullptr) {
            std::string identity = ":" + client.getNickname() + "!" + client.getUsername() + "@" + getHostname() + ": ";
            std::string mess = identity + buffer + "\r\n";
            send(tempClient.getClientFd(), mess.c_str(), mess.length(), 0);
        }
        else{
            std::string mess = NO_NICKNAME(client.getNickname() ,send_to);
            send(client.getClientFd(), mess.c_str(), mess.length(), 0);
        }
    }
}

int Server::checkChannel(Client &client, Channel &channel) {
    for (int i = 0; i < (int)channel.channelClients.size(); i++)
    {
        if (!channel.channelClients[i].getNickname().compare(client.getNickname()))
            return (1);
    }
    return (0);
}

Channel *Server::getFindChannel(std::string channel_name)
{
    for(int i = 0; i < (int)this->_channel_array.size(); i++)
    {
        if(this->_channel_array[i].getName() == channel_name)
        {
            return &this->_channel_array[i];
        }
    }
    return nullptr;
}

void Server::checkOperator(Channel &channel, Client &client)
{
    for(int i = 0; i < (int)channel.getOperatorArray().size(); i++)
    {
        if(channel.getOperatorArray()[i].getNickname() == client.getNickname())
            channel.getOperatorArray().erase(channel.getOperatorArray().begin() + i);
    }
    if(channel.getOperatorArray().size() == 0 && channel.channelClients.size() != 0)
        channel.addOperator(channel.channelClients[0]);
}

void Server::deleteChannelClient(int client_fd, Channel &channel)
{
    for(int j = 0; j < (int)channel.channelClients.size(); j++)
    {
        if(channel.channelClients[j].getClientFd() == client_fd){
            channel.channelClients.erase(channel.channelClients.begin() + j);
            checkOperator(channel, *findClient_WFD(client_fd));
        }
    }
    if(channel.channelClients.size() == 0)
    {
        for(int i = 0; i < (int)_channel_array.size(); i++)
        {
            if(channel.getName() == _channel_array[i].getName())
                _channel_array.erase(_channel_array.begin() + i);
        }
    }
}

void Server::clearChannelClient(int client_fd)
{
    for(int i = 0; i < (int)this->_channel_array.size(); i++)
    {
        deleteChannelClient(client_fd, this->_channel_array[i]);
    }
}

void Server::partCommand(Client *client, std::string part_name)
{
    Channel &tempChannel = *getFindChannel(part_name);
    if(&tempChannel != nullptr)
    {
        if(checkChannel(*client, tempChannel))
        {
            deleteChannelClient(client->getClientFd(), tempChannel);
            std::string mess = ":" + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname() + " PART " + tempChannel.getName() + "\r\n";
            send(client->getClientFd(), mess.c_str(), mess.length(), 0);
        }
        else
        {
            std::string mess = NOTONCHANNEL(client->getNickname(), tempChannel.getName());
            send(client->getClientFd(), mess.c_str(), mess.length(), 0);
        }
    }
    else
    {
        std::string mess = NO_SUCH_CHANNEL(client->getNickname(), part_name);
        send(client->getClientFd(), mess.c_str(), mess.length(), 0);
    }
}

void Server::kickCommand(Client &client, std::string channel_name, std::string client_to_kick, int i)
{
    Channel &tempChannel = *getFindChannel(channel_name);
    Client &clientToKick = *findClient_WNAME(client_to_kick);
    Client &tempClient = *findClient_WFD(i);
    if(&tempChannel != nullptr)
    {
        if(&clientToKick != nullptr)
        {
            for (int i = 0; i < (int)tempChannel.getOperatorArray().size(); i++)
            {
                if(tempChannel.getOperatorArray()[i].getClientFd() == tempClient.getClientFd())
                {
                    if(checkChannel(clientToKick, tempChannel))
                    {
                        deleteChannelClient(clientToKick.getClientFd(), tempChannel);
                        std::string mess = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " KICK " + tempChannel.getName() + " " + clientToKick.getNickname() + "\r\n";
                        send(clientToKick.getClientFd(), mess.c_str(), mess.length(), 0);
                        for(int i = 0; i < (int)tempChannel.channelClients.size(); i++)
                            send(tempChannel.channelClients[i].getClientFd(), mess.c_str(), mess.length(), 0);
                    }
                }
            }
        }
    }
}

void Server::topicCommand(int i, std::string channelName, std::string topicMessage) {
    Channel &tempChannel = *getFindChannel(channelName);
    Client &tempClient = *findClient_WFD(i);

    if(&tempChannel != nullptr)
    {
        if(&tempClient != nullptr)
        {
            for (int i = 0; i < (int)tempChannel.getOperatorArray().size(); i++)
            {
                if(tempChannel.getOperatorArray()[i].getClientFd() == tempClient.getClientFd())
                {
                    tempChannel.setTopic(topicMessage);
                    for (int k = 0; k < (int)tempChannel.channelClients.size(); k++) {
                        std::string msg =  ": 332 " + tempChannel.channelClients[k].getNickname() + " " + tempChannel.getName() + " : " + tempChannel.getTopic() + "\r\n";
                        send(tempChannel.channelClients[k].getClientFd(), msg.c_str(), msg.length(), 0);
                    }
                    return ;
                }
            }
        }
    }
    else
    {
        std::string msg = ": 403 : " + tempClient.getNickname() + " There is no channel\r\n";
        send(tempClient.getClientFd(), msg.c_str(), msg.length(), 0);
    }
}