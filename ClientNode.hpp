#ifndef CLIENTNODE_HPP
#define CLIENTNODE_HPP

#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h> 
#include <pthread.h>
#include <errno.h> 
#include <sys/types.h>
#include <netinet/in.h>  
#include <string>
#include <iostream>
#include <fstream>

#define PORT 60019 
#define LENGTH 101
#define NAME_LENGTH 31
#define maxClients 20
class ClientNode
{
    public:
        ClientNode();
        std::string clientName;
        int sockfd;
};


#endif // CLIENTNODE_HPP