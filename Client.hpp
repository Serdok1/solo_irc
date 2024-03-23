#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "IRC.hpp"

class Client {
    private:
        int             _client_fd;
        std::string     _nickname;
        std::string     _username;
        std::string     _real_name;
        std::string     _hostname;
    public:
        Client();
        Client(int client_fd);
        ~Client();

        int             getClientFd();
        std::string     getNickname();
        std::string     getUsername();
        std::string     getRealName();
        std::string     getHostname();
        
        void            setClientFd(int socket_fd);
        void            setNickname(std::string nickname);
        void            setUsername(std::string username);
        void            setRealName(std::string real_name);
        void            setHostname(std::string hostname);
};

#endif