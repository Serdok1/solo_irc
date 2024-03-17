#ifndef SERVER_HP
    #define SERVER_HPP

    #include "irc.hpp"

class Server{
    private:
        int _port_number;
        std::string _server_password;
    public:
        Server(int port_number, std::string server_password);
        ~Server();
        int             getPortNumber(void);
        std::string     getServerPassword(void);
        void            setPortNumber(int port_number);
        void            setServerPassword(std::string server_password);
};

#endif