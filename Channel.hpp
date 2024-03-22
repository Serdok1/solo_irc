#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"

class Client;

class Channel{
    private:
        std::string     _name;
        std::string     _password;
        std::string     _topic;
        int             _operator;
    public:
        Channel(std::string name, std::string password, Client &client);
        Channel(std::string name, std::string password);
        Channel();
        ~Channel();

        std::vector<Client>     channelClients;
        std::string             getName(void);
        std::string             getPassword(void);
        std::string             getTopic();
        int                     getOperator();
        void                    setName(std::string name);
        void                    setPassword(std::string password);
        void                    setTopic(std::string topic);
        void                    setOperator(int operatorFd);
};

#endif