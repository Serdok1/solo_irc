#include "irc.hpp"

int main(int ac, char **av){
    if(ac != 3){
        std::cout << "Wrong arguments! You have to run the server with <port_number> and <server_password>." << std::endl;
        return 1;
    }
    Server server(atoi(av[1]), av[2]);
    //Server socket things...
    
}