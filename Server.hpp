#ifndef SERVER_HPP
    #define SERVER_HPP

    #include "IRC.hpp"
    #include "Client.hpp"
    #include "Channel.hpp"

class Server{
    private:
        int                     _port_number;
        std::string             _server_password;
        struct sockaddr_in      _sockaddr;
        socklen_t               _sockaddr_len;
        int                     _sockfd;
        int                     _new_socket;
        std::vector<Client>     _client_array;
        std::vector<Channel>    _channel_array;
    public:
        Server(int port_number, std::string server_password);
        ~Server();
        //getter methods
        int                     getPortNumber(void);
        std::string             getServerPassword(void);
        struct sockaddr_in      getSocketAddress(void);
        socklen_t               getSocketAddressLength(void);
        int                     getNewSocket(void);
        int                     getSocketFd(void);
        std::vector<Client>     getClientArray(void);
        std::vector<Channel>    getChannelArray(void);
        //setter methods
        void                    setPortNumber(int port_number);
        void                    setServerPassword(std::string server_password);
        void                    setSocketAddress(struct sockaddr_in sockaddr);
        void                    setSocketAddressLength(socklen_t sockaddr_len);
        void                    setNewSocket(int new_socket);
        void                    setSocketFd(int sockfd);
        void                    setClientArray(std::vector<Client> clientArray);
        void                    setChannelArray(std::vector<Channel> channelArray);
        //other funcs
        int                     createSocket(void);
        int                     setSocketOpt(void);
        int                     setNonBlock(void);
        void                    setAddressProperties(sa_family_t family, in_addr_t address);
        int                     bindSocket(void);
        int                     listenSocket(void);
        int                     acceptSocket(void);
        void                    addNewClient(Client &client);
        void                    deleteClient(int socket_fd);
        Client                  *findClient(int socket_fd);
};

#endif