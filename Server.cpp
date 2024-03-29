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
    /* std::string mess;
    if((int)_channel_array.size() == 0) {
        _channel_array.push_back(Channel(channelName, channelPassword, client));
        _channel_array[0].addOperator(client);
        mess = "JOIN You are now in channel "  + channelName +" \r\n";

    }
    else {
        bool controlFlag1 = false;
        bool controlFlag2 = false;
        for (int i = 0; i < (int)_channel_array.size(); i++)
        {
            if(!_channel_array[i].getName().compare(channelName))
            {
                for (int k = 0; k < (int)_channel_array[i].channelClients.size(); k++)
                {
                    if (!_channel_array[i].channelClients[k].getNickname().compare(client.getNickname()))
                    {
                        controlFlag1 = true;
                        controlFlag2 = true;
                        mess = "JOIN You are now in channel "  + channelName +" \r\n";
                        return;
                    }
                }
                if(controlFlag1 == false)
                {
                    if((!_channel_array[i].getPassword().compare(channelPassword)))
                    { 
                        _channel_array[i].channelClients.push_back(client);
                        mess = "JOIN You are now in channel "  + channelName +" \r\n";
                    }
                    else
                        mess = BADCHANNEL(channelName);
                    controlFlag2 = true;
                }
                break;
            }
        }
        if(controlFlag2 == false)
        {
            _channel_array.push_back(Channel(channelName, channelPassword, client));
            _channel_array[(int)_channel_array.size() - 1].addOperator(client);
            mess = "JOIN You are now in channel "  + channelName +" \r\n";

        }
    }
    std::string buffer = ":" + client.getNickname() + "!" + client.getUsername() + "@" + getHostname() + ": "  +  mess + "\r\n";;
    send(client.getClientFd(), buffer.c_str(), buffer.length(), 0); */
    std::string mess;
    Channel &tempChannel = *getFindChannel(channelName);
    if(&tempChannel == nullptr)
    {
        _channel_array.push_back(Channel(channelName, channelPassword, client));
        _channel_array[0].addOperator(client);
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

void Server::deleteChannelClient(int client_fd, Channel &channel)
{
    //oper kontrolü
    for(int j = 0; j < (int)channel.channelClients.size(); j++)
    {
        if(channel.channelClients[j].getClientFd() == client_fd)
            channel.channelClients.erase(channel.channelClients.begin() + j);
    }
}

void Server::clearChannelClient(int client_fd)
{
    //oper kontrolü
    for(int i = 0; i < (int)this->_channel_array.size(); i++)
    {
        for(int j = 0; j < (int)this->_channel_array[i].channelClients.size(); j++)
        {
            if(this->_channel_array[i].channelClients[j].getClientFd() == client_fd)
                this->_channel_array[i].channelClients.erase(this->_channel_array[i].channelClients.begin() + j);
        }
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

void Server::kickCommand(Client &client, std::string channel_name, std::string client_to_kick)
{
    Channel &tempChannel = *getFindChannel(channel_name);
    Client &clientToKick = *findClient_WNAME(client_to_kick);
    if(&tempChannel != nullptr)
    {
        if(&clientToKick != nullptr)
        {
            if(checkChannel(clientToKick, tempChannel))
            {
                deleteChannelClient(clientToKick.getClientFd(), tempChannel);
                std::string mess = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.getHostname() + " KICK " + tempChannel.getName() + " " + clientToKick.getNickname() + "\r\n";
                send(clientToKick.getClientFd(), mess.c_str(), mess.length(), 0);
                for(int i = 0; i < (int)tempChannel.channelClients.size(); i++)
                    send(tempChannel.channelClients[i].getClientFd(), mess.c_str(), mess.length(), 0);
            }
            else
            {
                //channel içerisinde kicklenen client yok
            }
        }
        else
        {
            //kicklenecek client yok
        }
    }
    else
    {
        //channel yok
    }
}