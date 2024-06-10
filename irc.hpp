#ifndef IRC_HPP
    #define IRC_HPP
    

    #include <netinet/in.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <unistd.h>
    #include <fstream>
    #include <arpa/inet.h>
    #include <vector>
    #include <algorithm>
    #include <iterator>
    #include <sstream>
    #include <string>
    #include <iostream>
    #include <fcntl.h>

    #include "Messages.hpp"

    class   Server;
    class   Client;
    void    acceptSetClient(Server *server, fd_set &current_sockets, int &max_socket);
    void    authenticateClient(Client &client);
    bool    clientCanAuthenticate(Client &client);
    void    handleQuit(Server *server, int i, fd_set &current_sockets);
    void    handleClient(Server *server, int i, fd_set &current_sockets);
    void    handleNickCommand(std::istringstream &iss, Client &client);
    void    handlePassCommand(std::istringstream &iss, Client &client, Server *server);
    void    handleUserCommand(std::istringstream &iss, Client &client);
    void    handleJoinCommand(std::istringstream &iss, Client &client, Server *server);
    void    handlePrivmsgCommand(std::istringstream &iss, Client &client, Server *server, const std::string &buffer);
    void    handleNoticeCommand(std::istringstream &iss, Client &client, Server *server, const std::string &buffer);
    void    handlePartCommand(std::istringstream &iss, Client &client, Server *server);
    void    handleKickCommand(std::istringstream &iss, Client &client, Server *server, int clientSocket);
    void    handleTopicCommand(std::istringstream &iss, Server *server, int clientSocket);
    void    initServer(Server *server);

#endif