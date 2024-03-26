#ifndef SERVER_HPP
    #define SERVER_HPP

    #include "IRC.hpp"
    #include "Client.hpp"
    #include "Channel.hpp"

class Client;
class Channel;

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
        std::string             _hostname;
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
        std::string             getHostname(void);
        //setter methods
        void                    setPortNumber(int port_number);
        void                    setServerPassword(std::string server_password);
        void                    setSocketAddress(struct sockaddr_in sockaddr);
        void                    setSocketAddressLength(socklen_t sockaddr_len);
        void                    setNewSocket(int new_socket);
        void                    setSocketFd(int sockfd);
        void                    setClientArray(std::vector<Client> clientArray);
        void                    setChannelArray(std::vector<Channel> channelArray);
        void                    setHostname(std::string hostname);
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
        Client                  *findClient_WFD(int socket_fd);
        Client                  *findClient_WNAME(std::string name);
        void                    joinCommand(Client &client, std::string channelName, std::string channelPassword);
        void                    msgCommand(Client &client, std::string send_to, std::string buffer);
        Channel                 *findChannel(std::string channel_name);
        int                     checkChannel(Client &client, Channel &channel);
        void                    deleteChannelClient(int client_fd);
};

#endif