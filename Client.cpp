/*
 *      File  : Client.cpp
 *      Author: Stephen Lamalie
 *      Course: COP4635
 *      Proj  : Project 02
 */


#include "ClientNode.hpp"


void* sendIt(void*);
void* receiveIt(void*);
//void prompt();

//global variable exit flag
int exitFlag = 0;
int sock = 0;
char buffer[256] = {0};

int main(int argc, char *argv[]) 
{
  //  int n; 
    struct sockaddr_in serv_addr; 
    //char buffer[256] = {0}; 
    std::string clientName;
    char name[NAME_LENGTH];
    pthread_t tidR;
    pthread_t tidS;
   // struct timeval tv;
   // tv.tv_sec = 2;
  // tv.tv_usec = 0;

    if(argc != 2)
    {
        printf("Two arguments are required!");
        exit(1);
    }
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
   //using the loopback address for testing
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
  //char* message;
  //int p;
  
  std::cout << "Welcome to the chatroom!\nPlease enter your name: " << std::endl;
  std::cin >> clientName;
  while((clientName.size() < 2) || (clientName.size() > 30))
  {
    std::cout << "Please use a name between 2-30 characters long" << std::endl;
    std::cin >> clientName;
  }
  strcpy(name,clientName.c_str());
  send(sock,name,NAME_LENGTH,0);
  
  //gets(message);
  pthread_create(&tidR,NULL,&receiveIt,NULL);
  pthread_create(&tidS,NULL,&sendIt,NULL);
  //pthread_join(tidR, NULL);
  //pthread_join(tidS, NULL);
  
  

  while(1)
  {
    if(exitFlag == 1)
    {
      break;
    }
  }
   // int test;
   // while((test = select(sock+1, &sock, NULL, NULL, &tv)) > 0)
   // {
     ////recv( sock , buffer, sizeof(buffer),0); //recent 
   // }
   // printf("%d", test);
    ////printf("%s\n",buffer ); //recent
    close(sock);
    return 0; 
} 

void* sendIt(void* arg)
{
	//char message[LENGTH] = {0};
  
  while(1)
  {
    std::string message;
    //std::cin >> message;
    getline(std::cin,message);
    if(message.compare("quit") == 0)
    {
      exitFlag = 1;
      break;
    }
    else if(message.compare("\n") != 0)
    {
      
      send(sock,message.c_str(),message.size(),0);
    }
    //prompt();
    /*
    fgets(message,LENGTH,stdin);//while(fgets(message,LENGTH,stdin) != NULL)
    std::string mess(message);
     if(mess.compare(0,4,"quit") == 0)
    {
      exitFlag = 1;
      break;
    }
    if(strcmp(message,"\n") != 0)
    {
      send(sock,message,LENGTH,0);
    }
   */
  }
  //exitFlag = 1;
	pthread_exit(0);
}
void* receiveIt(void* arg)
{
  char message[LENGTH] = {0};

  while(1)
  {
    int rec = recv(sock,message,LENGTH,0);
    std::string mess(message);
    mess = mess.substr(0,rec); 
   
    if(rec > 0)
    {
     std::cout << std::endl << mess << std::endl;//printf("%s",message);
     ///prompt();
    }
    else if(rec == 0)
    {
      break;
    }
  }
  pthread_exit(0);
}
//void prompt()
//{

//  printf("$$$");
//  fflush(stdin);
//}