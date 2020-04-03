# Makefile for project 2. will make use of make to create the client and serevr excutiables, and Make clean
# to clean all .o and executables.

OBJECTS = Server.o Client.o ClientNode.o
CXXFLAGS = -g -c -Wall
CXX = g++ -pthread


all: $(OBJECTS)
	$(CXX)  Server.o ClientNode.o -o server
	$(CXX)  Client.o -o client

Client.o: Client.cpp
	$(CXX) $(CXXFLAGS) Client.cpp

Server.o: Server.cpp
	$(CXX) $(CXXFLAGS) Server.cpp

ClientNode.o: ClientNode.cpp
	$(CXX) $(CXXFLAGS) ClientNode.cpp

clean:
	$(RM) *o *gch server client
