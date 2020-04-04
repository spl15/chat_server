/*
 *      File  : Client.cpp
 *      Author: Stephen Lamalie
 *      Course: COP4635
 *      Proj  : Project 02
 */


#include "ClientNode.hpp"


void* sendIt(void*);
void* receiveIt(void*);
void dealWithFile(int);
void sendFile(std::string);
int getFileSize(std::string);

//global variable exit flag
int exitFlag = 0;
int sock = 0;
char buffer[8000] = {0};
int flag = 0;

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
    else if(message.compare(0,5,"FILE>") == 0)
    {
      sendFile(message);
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
  //char message[LENGTH] = {0};

  while(1)
  {
    int rec = recv(sock,buffer,sizeof(buffer),0);
    std::string mess(buffer);
    mess = mess.substr(0,rec);
    std::string fileFlag;
    int first = mess.find(':');
    int second = mess.find('>');
    fileFlag = mess.substr(first+1,5);
    if(rec > 0)
    {
      if(fileFlag.compare("FILE>") == 0)
      {
        dealWithFile(rec);
      }
      else
      {
        std::cout << std::endl << mess << std::endl;//printf("%s",message);
      }
    memset(buffer,0,sizeof(buffer)); 
    } 
    else if(rec == 0)
    {
      break;
    }
  }
  pthread_exit(0);
}
void dealWithFile(int theSize)
{
    // loads the data into the buffer and cuts out the header
    std::string fileData(buffer);
    int place = fileData.find(":");
    std::string fromWho = fileData.substr(0,place+1);

    int placeB = fileData.find(">");
    std::string head = fileData.substr(place+1,5);
    //std::string mess(fileData);
    //std::cout << mess << std::endl;
    //std::cout << theSize << std::endl;
    int plusPosition = fileData.find("+");
    std::string fileName((fileData.begin()+placeB+1),(fileData.begin()+plusPosition));//std::string fileName = fileData.substr(placeB+1, spacePosition);
    //memset(buffer,0,sizeof(buffer));
    //fileData = fileData.substr(fromWho.size()+head.size()+fileName.size()+1);
    //strcpy(buffer,fileData.c_str());
    //std::cout << fromWho << std::endl; 
    //std::cout << placeB << std::endl;
    //std::cout << plusPosition << std::endl;
    //std::cout << head << std::endl;
    //std::cout << fileName << std::endl;
    // write the file
    printf("File: %s received\n", fileName.c_str());
    FILE* theFile;
    // create a file with the contents of the buffer
    theFile = fopen(fileName.c_str(), "w+");
    int i = 0;
    char c;

    for(i = plusPosition+1;i < (theSize-plusPosition+1);i++)
    {
     
	    c = buffer[i];
	    fputc(c, theFile);
            
    }
    //close the file
    fclose(theFile);
}
void sendFile(std::string header)
{
  //find the filename
  //memset(buffer,0,sizeof(buffer));
  int posA = header.find(">");
  int pos = header.find(">",posA+1);
  std::string myFileName = header.substr(pos+1);
  std::string newHeader = header + "+";
  memset(buffer,0,sizeof(buffer));
  //copy the header into the buffer
  strcpy(buffer,newHeader.c_str());
  //copy the rest of the file into the buffer
   int fileSize = getFileSize(myFileName);   
  //std::ifstream fileName(myFileName);
  // reads a file whos name is specified by the header
  FILE* fileName;
  fileName = fopen(myFileName.c_str(), "rb");
  std::cout << myFileName.c_str() << std::endl;
  if(fileName != NULL)
  {
    //find the file size to send.
    //fseek(fileName, 0L, SEEK_END);
    //int fileSize = (int)ftell(fileName);
	  //rewind(fileName);
    //int fileSize = getFileSize(myFileName);    
	  int i = 0;
	  int c;
    /*std::streambuf * pbuf = fileName.rdbuf();
    do {
      char ch = pbuf->sgetc();
      buffer[i + (sizeof(newHeader)-1)] = ch;
    } while ( pbuf->snextc() != EOF );
    *///istr.close();       
    // send the file
    int position = newHeader.find("+");
	  for(i = 0;i < fileSize;i++)
	  {
	    c = getc(fileName);
	    buffer[i+position+1] = c;//buffer[i + (sizeof(newHeader)-1)] = c;//EDIT 
	  }

    //fclose(fileName);
    fclose(fileName);
    int combo = fileSize + newHeader.size();
    int sentSize = send(sock,buffer,combo,0);
    std::cout << "file sent->" << sentSize << std::endl;
    std::cout << fileSize << "+" << newHeader.size() << std::endl;
  }
  else
  {
    //lets the user know he does not have such a file and that is all
    std::cout << "you dont have a file by that name" << std::endl;
    std::cout << myFileName << std::endl;
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