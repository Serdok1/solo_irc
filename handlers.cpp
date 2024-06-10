#include "IRC.hpp"
#include "Server.hpp"

void handleClient(Server *server, int clientSocket, fd_set &current_sockets) {
    char buffer[1028] = {0};
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead <= 0) {
        if (bytesRead == 0) {
            std::cout << "client left the server, socket no: " << clientSocket << std::endl;
        } else {
            perror("recv error");
        }
        handleQuit(server, clientSocket, current_sockets);
        return;
    }

    buffer[bytesRead] = '\0';
    std::cout << "----------\n" << buffer << "----------" << std::endl;

    Client &client = *server->findClient_WFD(clientSocket);
    std::istringstream iss(buffer);
    std::string token;

    while (iss >> token) {
        if (token == "NICK") {
            handleNickCommand(iss, client);
        } else if (token == "PASS") {
            handlePassCommand(iss, client, server);
        } else if (token == "USER") {
            handleUserCommand(iss, client);
        } else if (token == "JOIN") {
            handleJoinCommand(iss, client, server);
        } else if (token == "PRIVMSG") {
            handlePrivmsgCommand(iss, client, server, buffer);
        } else if (token == "NOTICE") {
            handleNoticeCommand(iss, client, server, buffer);
        } else if (token == "PART") {
            handlePartCommand(iss, client, server);
        } else if (token == "KICK") {
            handleKickCommand(iss, client, server, clientSocket);
        } else if (token == "TOPIC") {
            handleTopicCommand(iss, server, clientSocket);
        } else if (token == "QUIT") {
            handleQuit(server, clientSocket, current_sockets);
        }

        if (clientCanAuthenticate(client)) {
            authenticateClient(client);
        }
    }
}

void handleNickCommand(std::istringstream &iss, Client &client) {
    if (!client.getIsAuth()) {
        std::string nick;
        iss >> nick;
        client.setNickname(nick);
    }
}

void handlePassCommand(std::istringstream &iss, Client &client, Server *server) {
    if (!client.getIsAuth() && client.getPassword().empty()) {
        std::string pass;
        iss >> pass;
        if (!pass.empty() && pass[0] == ':') pass.erase(0, 1);

        if (!pass.empty() && pass == server->getServerPassword()) {
            client.setPassword(pass);
        } else {
            std::string mess = PASS_ERR(client.getNickname());
            send(client.getClientFd(), mess.c_str(), mess.length(), 0);
        }
    }
}

void handleUserCommand(std::istringstream &iss, Client &client) {
    if (!client.getIsAuth()) {
        std::string user;
        iss >> user;
        client.setUsername(user);
    }
}

void handleJoinCommand(std::istringstream &iss, Client &client, Server *server) {
    if (client.getIsAuth()) {
        std::string channelName, channelPassword;
        iss >> channelName >> channelPassword;

        if (channelName[0] != '#' && !channelName.empty()) channelName = "#" + channelName;

        if (!channelName.empty() && !channelPassword.empty()) {
            server->joinCommand(client, channelName, channelPassword);
        } else {
            send(client.getClientFd(), "missing password\r\n", 19, 0);
        }
    }
}

void handlePrivmsgCommand(std::istringstream &iss, Client &client, Server *server, const std::string &buffer) {
    if (client.getIsAuth()) {
        std::string send_to;
        iss >> send_to;
        server->msgCommand(client, send_to, buffer);
    }
}

void handleNoticeCommand(std::istringstream &iss, Client &client, Server *server, const std::string &buffer) {
    if (client.getIsAuth()) {
        std::string send_to;
        iss >> send_to;
        server->msgCommand(client, send_to, buffer);
    }
}

void handlePartCommand(std::istringstream &iss, Client &client, Server *server) {
    if (client.getIsAuth()) {
        std::string partName;
        iss >> partName;

        if (partName[0] != '#' && !partName.empty()) partName = "#" + partName;

        if (!partName.empty()) {
            server->partCommand(&client, partName);
        }
    }
}

void handleKickCommand(std::istringstream &iss, Client &client, Server *server, int clientSocket) {
    if (client.getIsAuth()) {
        std::string channelName, nickToKick;
        iss >> channelName >> nickToKick;

        if (channelName[0] != '#' && !channelName.empty()) channelName = "#" + channelName;

        if (!channelName.empty() && !nickToKick.empty()) {
            server->kickCommand(client, channelName, nickToKick, clientSocket);
        }
    }
}

void handleTopicCommand(std::istringstream &iss, Server *server, int clientSocket) {
    std::string temp, topicMessage, channelName;
    iss >> channelName;

    if (!channelName.empty() && channelName[0] != '#') channelName = "#" + channelName;

    while (iss >> temp) {
        topicMessage += temp + " ";
    }

    if (!topicMessage.empty() && !channelName.empty()) {
        server->topicCommand(clientSocket, channelName, topicMessage);
    }
}

void handleQuit(Server *server, int i, fd_set &current_sockets)
{
    close(i);
    server->clearChannelClient(i);
    server->deleteClient(i);
    FD_CLR(i, &current_sockets);
}

/* void handleClient(Server *server, int i, fd_set &current_sockets){
    char buffer[1028] = {0};
    size_t read_value;
    if((read_value = recv(i, buffer, sizeof(buffer) - 1, 0)) <= 0)
    {
        if(read_value == 0)
            std::cout << "client left the server, socket no: " << i << std::endl;
        else
            perror("recv error");
        handleQuit(server, i, current_sockets);
    }else{
        buffer[read_value] = '\0';
        std::cout << "----------\n" << buffer << "----------" << std::endl;
        std::istringstream iss(buffer);
        std::string token, user, pass, nick;
        Client &tempClient = *server->findClient_WFD(i);
        while (iss >> token) {
            if (token == "NICK" && !tempClient.getIsAuth()){
                iss >> nick;
                tempClient.setNickname(nick);
            }
            else if (token == "PASS" && !tempClient.getIsAuth() && tempClient.getPassword().empty())
            {
                iss >> pass;
                if (!pass.empty() && pass[0] == ':')
                    pass.erase(0, 1);
                if(!pass.empty() && !pass.compare(server->getServerPassword()))
                    tempClient.setPassword(pass);
                else
                {
                    std::string mess = PASS_ERR(tempClient.getNickname());
                    send(tempClient.getClientFd(), mess.c_str(), mess.length(), 0);
                }
            }
            else if (token == "USER" && !tempClient.getIsAuth()){
                iss >> user;
                tempClient.setUsername(user);
            }
            else if (token == "JOIN" && tempClient.getIsAuth()) {
                std::string channelName, channelPassword;
                iss >> channelName;
                iss >> channelPassword;
                if (channelName[0] != '#' && !channelName.empty())
                    channelName = "#" + channelName;
                if (!channelName.empty() && !channelPassword.empty())
                    server->joinCommand(tempClient, channelName, channelPassword);
                else
                    send(tempClient.getClientFd(), "missing password\r\n", 19, 0);
            }
            else if (token == "PRIVMSG" && tempClient.getIsAuth()) {
                std::string send_to;
                iss >> send_to;
                server->msgCommand(tempClient, send_to, buffer);
            }
            else if (token == "NOTICE" && tempClient.getIsAuth())
            {
                std::string send_to;
                iss >> send_to;
                server->msgCommand(tempClient, send_to, buffer);
            }
            else if(token == "PART" && tempClient.getIsAuth())
            {
                std::string partName;
                iss >> partName;
                if (partName[0] != '#' && !partName.empty())
                    partName = "#" + partName;
                if(!partName.empty())
                    server->partCommand(&tempClient, partName);
            }
            else if(token == "KICK" && tempClient.getIsAuth())
            {
                std::string channelName, nickToKick;
                iss >> channelName;
                if (channelName[0] != '#' && !channelName.empty())
                    channelName = "#" + channelName;
                iss >> nickToKick;
                if (!channelName.empty() && !nickToKick.empty())
                    server->kickCommand(tempClient, channelName, nickToKick, i);
            }
            else if (token == "TOPIC") {
                std::string temp, topicMessage, channelName;
                iss >> channelName;
                if (!channelName.empty() && channelName[0] != '#')
                    channelName = "#" + channelName;
                while (iss >> temp) {
                    topicMessage += temp;
                    topicMessage += " ";
                }
                if (!topicMessage.empty() && !channelName.empty()) {
                    server->topicCommand(i, channelName, topicMessage);
                }
            }
            else if(token == "QUIT"){
                handleQuit(server, i, current_sockets);
            }
            if(!tempClient.getIsAuth() && !tempClient.getPassword().empty() && !tempClient.getUsername().empty() && !tempClient.getNickname().empty())
            {
                tempClient.setIsAuth(true);
                std::string message = LOGIN(tempClient.getNickname(), tempClient.getUsername());
                send(tempClient.getClientFd(), message.c_str(), message.length(), 0);
            }
        }
    }
} */