#ifndef MESSAGES_HPP
#define MESSAGES_HPP

#include "Client.hpp"
#include "Server.hpp"

#define LOGIN(nickname, username) (": 001 " + nickname + " :Welcome to the IRC Network " + nickname + "!" + username + "\r\n")
#define PASS_ERR(nickname) (": 464 " + nickname + " :Password incorrect! Please check your password\r\n")
#define NO_NICKNAME(nickname) (": 431 " + nickname + " :No nickname given\r\n")
#define NICKNAME_IN_USE(nickname) (": 433 " + nickname + " :Nickname is already in use. Please provide your password with 'USERPASS: <password> if you have this nickname already!'\r\n")
#define BADCHANNEL(channel) (": 475 " + channel + " :Incorrect password for channel!\r\n")
#define ERR_PASSWDMISMATCH(nick) (": 464 " + nick + "  : password supplied does not match the password expected by the server!\r\n")
#define ERR_ALREADYREGISTERED(nickname, username) (": 462 " + nickname + "  \r\n"  )


#endif