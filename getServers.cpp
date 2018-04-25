#include "NIM.h"
#include <WinSock2.h>
#include <iostream>

// getServers.cpp
//  Given a socket, a broadcast address and a port number, this function returns the number
//  of TicTacToe servers as well as an array of structs.  Each struct contains the name, IP_Address and 
//  port number of a remote TicTacToe server.

//	IN parameters:
//	SOCKET s				= Allocated UDP socket handle that can be used for communcations
//	char *broadcastAddress	= The broadcast address that should be used for the current LAN
//	char *broadcastPort		= The port that should be used for the broadcast

//	OUT parameters:
//	ServerStruct serverArray[]	= An array that contains the structs.  The members of each struct
//							  contain the name, IP Address and port number of a remote TicTacToe server.
//	numServers				= Number of elements in serverArray[]

//	Function return value	= Number of remote TicTacToe servers (size of serverArray[])

int getServers(SOCKET s, char *broadcastAddress, char *broadcastPort, ServerStruct serverArray[]) {
    // This function returns the number of TicTacToe servers that may be found within the current Broadcast Domain.
    // The last parameter, serverArray[], will contain all of the servers' names, IP addresses and port numbers.

    int numServers = 0;

    // Send TicTacToe_QUERY to broadcastAddress using broadcastPort
    /****
    Task 3: Add code here that will send the TicTacToe_QUERY message to the broadcastAddress using the broadcastPort (see function header).
    ****/
    char buffer[MAX_SEND_BUF];
    strcpy_s(buffer, NIM_QUERY);
    int numberCHarectersSent = UDP_send(s, buffer, strlen(NIM_QUERY) + 1, broadcastAddress, broadcastPort);


    // Receive incoming UDP datagrams (with a maximum of 2 second wait before each UDP_recv() function call
    // As you read datagrams, if they start with the prefix: TicTacToe_NAME, parse out the server's name
    // and add the name, host address and port number to serverArray[].  Don't forget to increment numServers.
    int status = wait(s, 2, 0);
    if (status > 0) {
        int len = 0;
        char recvBuffer[MAX_RECV_BUF + 1];
        char host[v4AddressSize];
        char port[portNumberSize];
        /****
        Task 4a: Add code here that will receive a response to the broadcast message
        ****/
        len = UDP_recv(s, recvBuffer, MAX_RECV_BUF, host, port);

        while (status > 0 && len > 0) {
            /****
            Task 4b: Inside this while loop, extract a response, which should be a C-string that looks like "Name=some server's name".
            If the response doesn't begin with the characters, "Name=", ignore it.
            If it does begin with the characters, "Name=", parse the actual name that follows and
            (i) assign that name to the array of structs, serverArray[numServers].name
            (ii) assign the IP Address from which the response originated to serverArray[numServers].host
            (iii) assign the server's port number to serverArray[numServers].port
            (iv) increment numServers
            ****/
            std::string mystring = recvBuffer;
            if (mystring.substr(0, 5) == NIM_NAME) {
                std::string serverName = mystring.substr(5, mystring.length() - 5);
                serverArray[numServers].name = serverName;
                serverArray[numServers].host = host;
                serverArray[numServers].port = port;
                numServers++;

            }
            // Now, we'll see if there is another response.
            status = wait(s, 2, 0);
            if (status > 0)
                len = UDP_recv(s, recvBuffer, MAX_RECV_BUF, host, port);
        }
    }
    return numServers;
}
