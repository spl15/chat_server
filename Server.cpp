/*
 *      File  : Server.cpp
 *      Author: Stephen Lamalie
 *      Course: COP4635
 *      Proj  : Project 02
 */ 
#include "ClientNode.hpp"

std::string login(std::string,int);
int enterPassword(std::string,int);
std::string reg(int);
void sendToAll(int);
void sendToOne(int,std::string);
std::string recString(int);
int privateSend(std::string,int);
void processFile(int,int);
int findUser(std::string);
void sendFile(int,int,std::string);
int getFileSize(std::string);


// declare what i need to be global 
char buffer[7000];
ClientNode guys[maxClients];

int main(int argc , char *argv[])   
{   
     int opt = 1;
    int masterSocket , addrlen , newSocket, activity , valread , sd, copy;   
    //int clientSocket[maxClients];   
    struct sockaddr_in address;
         
    //set of socket descriptors  
    fd_set readfds;   
         
    //create a master socket  
    if( (masterSocket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }   
    //set master socket to allow multiple connections ,  
    //this is just a good habit, it will work without this  
    if( setsockopt(masterSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
          sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   

    //type of socket created  
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( PORT );   
      
    //bind the socket to localhost port 60019 
    if (bind(masterSocket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }   
    printf("Listener on port# %d \n", PORT);   
         
    // max 5 pending connections for the master socket  
    if (listen(masterSocket, 5) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
         
    //accept the incoming connection  
    addrlen = sizeof(address);   
    printf("Waiting for connections...\n");  
   
    for(;;)   
    {   
        //clear the socket set  
        FD_ZERO(&readfds);   
     
        //add master socket to set  
        FD_SET(masterSocket, &readfds);   
        copy = masterSocket;   
             
        //add child sockets to set  
        for (int i = 0 ; i < maxClients ; i++)   
        {   
            //socket descriptor  
            sd = guys[i].sockfd;//sd = clientSocket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //highest file descriptor number, need it for the select function  
            if(sd > copy)   
                copy = sd;   
        }   
     
        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( copy + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }   
             
        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(masterSocket, &readfds))   
        {   
            if ((newSocket = accept(masterSocket,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
            //inform user of socket number - used in send and receive commands  
            printf("New connection , ip address: %s\n", inet_ntoa(address.sin_addr));  
           
                 
            //add new socket to array of sockets  
            for (int i = 0; i < maxClients; i++)   
            {   
                //if position is empty  
                if( guys[i].sockfd == 0 )   
                {   
                    guys[i].sockfd = newSocket;   
                       
                    int nameLength = recv(guys[i].sockfd,buffer,sizeof(buffer),0);
                    std::string myName(buffer);
                    myName = myName.substr(0,nameLength);
                    guys[i].clientName = myName;
                    
                    memset(buffer,0,sizeof(buffer));
                    std::cout << "adding " << guys[i].clientName << " to list of sockets as " << i << std::endl;
                                     
                    std::string cltName = guys[i].clientName;
                    guys[i].clientName = login(cltName, i);
                    
                    // send a greetign when a person sucessfully enter the login process
                    sendToOne(i,"Welcome to stephen Lamalie project chatroom!");
                    // send a message to everyone that person X has entered the chat
                    std::string message = " has joined the chat.";
                    strcpy(buffer,message.c_str());
                    sendToAll(i);
                    
                    break;   
                }   
            }   

        }   
             
        //else its some IO operation on some other socket 
        for (int i = 0; i < maxClients; i++)   
        {   
            sd = guys[i].sockfd;   
                 
            if (FD_ISSET( sd , &readfds))   
            {   
                //Check if it was for closing , and also read the  
                //incoming message  
                if ((valread = read( sd , buffer, sizeof(buffer))) == 0)   
                {   
                    //Somebody disconnected , get his details and print  
                    getpeername(sd , (struct sockaddr*)&address , 
                        (socklen_t*)&addrlen);   
                    std::cout << guys[i].clientName;
                    printf(" disconnected , ip %s \n" ,inet_ntoa(address.sin_addr)); 
           
                    //Close the socket and mark as 0 in list for reuse  
                    close( sd );   
                    guys[i].sockfd = 0;   
                }   
                     
                //Echo back the message that came in  
                else 
                {   
                    //set the string terminating NULL byte on the end  
                    //of the data read  
                    buffer[valread] = '\0';
                    std::string message(buffer);
                    message = message.substr(0,valread);
                    std::string test = message.substr(0,1);
                    std::string testF = message.substr(0,4);
                    // tests to see if : is the first char
                    if(test.compare(":") == 0)
                    {
                        int findMe = message.find(':',1);
                        std::string partOne = message.substr(1,findMe-1);
                        std::string partTwo = message.substr(findMe+1);
                        memset(buffer,0,sizeof(buffer));
                        strcpy(buffer,partTwo.c_str());
                        if(privateSend(partOne,i) == 1)
                        {
                            std::string reply = "Im sorry we could not find anyone by the name of " + partOne;
                            reply += " logged in";
                            memset(buffer,0,sizeof(buffer));
                            strcpy(buffer,reply.c_str());
                            send(guys[i].sockfd,buffer,reply.size(),0);
                            memset(buffer,0,sizeof(buffer));
                        }
                    }
                    else if(testF.compare("FILE") == 0)
                    {
                        //std::cout << "got here" << std::endl;
                        processFile(i,valread);
                    }
                    else
                    {
                        sendToAll(i);
                    }
              
                }   
            }   
        }   
    }   
         
    return 0;   
}   
void sendToOne(int i, std::string incoming)
{
        
    std::string buff = "(server): ";
    std::string extra = incoming;
    buff += extra;
    memset(buffer,0,sizeof(buffer));
    strcpy(buffer,buff.c_str());
  
    send(guys[i].sockfd , buffer , strlen(buffer) , 0 );//send(sd , buffer , strlen(buffer) , 0 );
    
    memset(buffer,0,sizeof(buffer));  
}
std::string recString(int i)
{
    memset(buffer,0,sizeof(buffer));
    int theLength = recv(guys[i].sockfd,buffer,sizeof(buffer),0);
    std::string response(buffer);
    response = response.substr(0,theLength);
    memset(buffer,0,sizeof(buffer));

    return response;
}
int privateSend(std::string name,int pos)
{
    int found = 0;
    std::string mess = "PRIVATE MESSAGE->(" + guys[pos].clientName + "): ";
    std::string extra(buffer);
    mess += extra;
    memset(buffer,0,sizeof(buffer));
    strcpy(buffer,mess.c_str());
    for(int j=0;guys[j].sockfd != 0;j++)
    {
        if(name.compare(guys[j].clientName) == 0)
        {
            send(guys[j].sockfd,buffer,mess.size(),0);
            found = 1;
        }
    }
    if(found == 1 )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
void sendToAll(int i)
{
    int count = 0;
    for(int j = 0; guys[j].sockfd != 0;j++)
    {
        if(i != j)
        {
            if(count == 0)
            {
                std::string buff = "(" + guys[i].clientName + "):";
                std::string extra(buffer);
                buff += extra;

                memset(buffer,0,sizeof(buffer));
                strcpy(buffer,buff.c_str());
                ++count;
            }
            send(guys[j].sockfd , buffer , strlen(buffer) , 0 );//send(sd , buffer , strlen(buffer) , 0 );
        }
    } 
    memset(buffer,0,sizeof(buffer));  
}
int enterPassword(std::string password,int pos)
{
    std::string myPassword;
    sendToOne(pos,"Please enter your password: ");
    myPassword = recString(pos);
    while(strcmp(password.c_str(),myPassword.c_str()) != 0)
    {
        sendToOne(pos,"Im sorry that password is incorrect, please try again, enter \"exit\" to go back");
        myPassword = recString(pos);
        if(strcmp(password.c_str(),myPassword.c_str()) == 0)
        {
            return 1;
        }
        if(strcmp(myPassword.c_str(),"exit") == 0)
        {
            break;
        }
    }
    if(strcmp(myPassword.c_str(),password.c_str()) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}
std::string reg(int pos)
{
    std::ifstream inputTextFile("passwords.txt");
    std::ofstream outputTextFile("passwords.txt",std::ios::app);
    int passwordMatch = 0;
    std::string userName = "FILEERROR";
    std::string password;
    if(inputTextFile.is_open())
    {
        std::string userName;
        std::string password;
        while((userName.size() < 2) || (userName.size() > 30))
        {
            sendToOne(pos,"Please enter a userName between 2-30 characters");
            userName = recString(pos);
        }
        outputTextFile << "\n" << userName << ",";
        while(passwordMatch == 0)
        {
            sendToOne(pos,"Please enter a password: ");
            password = recString(pos);
            std::string match;
            sendToOne(pos,"Please verify the password a second time: ");
            match = recString(pos);
            if(strcmp(match.c_str(),password.c_str()) == 0)
            {
                passwordMatch = 1;
            }
            else
            {
                sendToOne(pos,"Please try again");
            }
        }
        outputTextFile << password;
    }
    inputTextFile.close();
    outputTextFile.close();

    return userName;
}
std::string login(std::string name, int pos)
{
    std::string line;
    int userFound = 0;
    int validPassword = 0;
    std::string userid;
    std::string myName = name;
    while((userFound == 0) || (validPassword == 0))
    {
        std::ifstream inputTextFile("passwords.txt");
        //check if the file is open
        if(inputTextFile.is_open())
        {
            while(getline(inputTextFile, line))
            {
                //finds the first space position
                int commaPosition = line.find(",");
                //stores the first word in line 
                std::string userID = line.substr(0,commaPosition);
                //stores the next column assumed to be password field
                userid = userID;
                ++commaPosition;
                std::string password =line.substr(commaPosition);
                if(strcmp(userID.c_str(),myName.c_str()) == 0)
                {
                    userFound = 1;
                    if(enterPassword(password,pos) == 1)
                    {
                        validPassword =1;
                    }
                    
                }            
            }
            inputTextFile.close();
            if((userFound == 0) || (validPassword == 0))
            {
                std::string answer;
                while((strcmp(answer.c_str(),"register") != 0) && (strcmp(answer.c_str(),"try") != 0))
                {
                    sendToOne(pos,"unable to find your username, enter \"register\" to register or \"try\" to try another username");
                    answer = recString(pos);
                    
                    if(strcmp(answer.c_str(),"register") == 0)
                    {   
                        myName = reg(pos);
                        break;
                    }
                    else if(strcmp(answer.c_str(),"try") == 0)
                    {
                        sendToOne(pos,"What name would you like to try?");
                        myName = recString(pos);
                    }
                }
            }
        }
        else
        {
            std::cerr << "Unable to open file"<< std::endl;        
        }
    }

    return myName;
}
void processFile(int pos,int sizeData)
{   
    std::string nameSend;
    std::string nameFrom = guys[pos].clientName;

    std::string header(buffer);
    int firstPos = header.find(">");
    int secondPos = header.find(">",firstPos+1);
    int plusPos = header.find("+");
    std::string nameOfFile = header.substr(secondPos+1,(plusPos-secondPos-1));
    int sec = secondPos - firstPos - 1;
    nameSend = header.substr(firstPos+1,(sec));
    std::string sendHead = "(" + nameFrom + "):";

    FILE* theFile;
    // create a file with the contents of the buffer
    //std::cout << header.size() << std::endl;
    theFile = fopen("file_in_transmittion.txt", "w+");
    int i = 0;
    char c;
    int fileData = sizeData - (plusPos+1);
    for(i = 0;i < fileData;i++)
    {
     
	    c = buffer[i+plusPos+1];
	    fputc(c, theFile);
            
    }

    sendHead += header;

    //close the file
    fclose(theFile);

    
    std::string response;
    std::string res = nameFrom + " would like to send you a file, respond yes to accept or no to decline.";
    int tester = findUser(nameSend);
    //std::cout << tester << std::endl;
    //std::cout << nameSend << std::endl;

    if(tester != -1)
    {
        sendToOne(tester,res);
        
        response = recString(tester);
        
    }
    if(response.compare("yes") == 0)
    {
        sendFile(pos,tester,nameOfFile);
    }

}
int findUser(std::string search)
{
    int found = 0;
    int foundWhat;
    for(int i = 0;i < maxClients;i++)
    {
        //std::cout << search << "+" << guys[i].clientName  << std::endl;
        if(guys[i].clientName.compare(search) == 0)
        {
            found = 1;
            foundWhat = i;
        }
    }
    if(found == 1)
    {
        return foundWhat;
    }
    else
    {
        return -1;
    }   
}
void sendFile(int from,int to,std::string name)
{   
  std::string head = "(" + guys[from].clientName + "):FILE>";  
  std::string newHeader = head + name + "+";

  memset(buffer,0,sizeof(buffer));
  //copy the header into the buffer
  strcpy(buffer,newHeader.c_str());
  //copy the rest of the file into the buffer
   int fileSize = getFileSize(name);   
  // reads a file whos name is specified by the header
  FILE* fileName;
  fileName = fopen("file_in_transmittion.txt", "rb");
  if(fileName != NULL)
  {
     
	  int i = 0;
	  int c;
    
    int position = newHeader.find("+");
	  for(i = 0;i < fileSize;i++)
	  {
	    c = getc(fileName);
	    buffer[i+position+1] = c;
	  }

    
    fclose(fileName);
    int combo = fileSize + newHeader.size();
    send(guys[to].sockfd,buffer,combo,0);
  }

}
int getFileSize(std::string fileName)
{
    std::ifstream file(fileName.c_str(), std::ifstream::in | std::ifstream::binary);

    if(!file.is_open())
    {
        return -1;
    }

    file.seekg(0, std::ios::end);
    int fileSize = file.tellg();
    file.close();

    return fileSize;
}