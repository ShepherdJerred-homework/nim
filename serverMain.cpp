// server_main.cpp
//   This function serves as the "main" function for the server-side
#include "NIM.h"
#include <iostream>
#include <string>
#include <WinSock2.h>

int serverMain(int argc, char *argv[], std::string playerName) {
    SOCKET s;
    char buffer[MAX_RECV_BUF];
    std::string host;
    std::string port;
    char responseStr[MAX_SEND_BUF];

    s = passivesock(NIM_UDPPORT, "udp");

    std::cout << std::endl << "Waiting for a challenge..." << std::endl;
    int len = UDP_recv(s, buffer, MAX_RECV_BUF, (char *) host.c_str(), (char *) port.c_str());
    std::cout << timestamp() << " - Received: " << buffer << std::endl;

    bool finished = false;
    while (!finished) {
        if (strcmp(buffer, NIM_QUERY) == 0) {
            // Respond to name query
            strcpy_s(responseStr, NIM_NAME);
            strcat_s(responseStr, playerName.c_str());
            UDP_send(s, responseStr, strlen(responseStr) + 1, (char *) host.c_str(), (char *) port.c_str());
            std::cout << timestamp() << " - Sending: " << responseStr << std::endl;

        } else if (strncmp(buffer, NIM_CHALLENGE, strlen(NIM_CHALLENGE)) == 0) {
            // Received a challenge
            char *startOfName = strstr(buffer, NIM_CHALLENGE);
            if (startOfName != NULL) {
                std::cout << std::endl << "You have been challenged by " << startOfName + strlen(NIM_CHALLENGE)
                          << std::endl;
                /*

                insert code to:
                If the server receives a datagram that contains a string similar to �Player=client_name�, your code should inform
                your user that they have been challenged to a game of NIM by client_name. Give your user a chance to accept the
                challenge. If they refuse the challenge, send a UDP datagram back to the client containing the string �NO�, and then
                continue to listen for incoming traffic on port #29333.
                If they do accept the challenge, your server code should send a UDP datagram back to the client that contains the
                string �YES�, and then wait (for up to 2 seconds) for a reply UDP-datagram from the client that contains the string
                �GREAT!�. The server code is now ready to play the game.
                If the �GREAT!� datagram is not received within 2 seconds, your code should assume that the client computer isn�t
                ready to play, and resume listening on the UDP socket (#29333) for additional incoming traffic.

                */

                std::cout << std::endl << "Accept the challenge? ";
                std::string answerStr;
                std::getline(std::cin, answerStr);
                if (answerStr[0] == 'y' || answerStr[0] == 'Y') {
                    strcpy_s(responseStr, NIM_ACCEPT);
                    UDP_send(s, responseStr, strlen(responseStr) + 1, (char *) host.c_str(), (char *) port.c_str());

                    int len = UDP_recv(s, buffer, MAX_RECV_BUF, (char *) host.c_str(), (char *) port.c_str());

                    if (strcmp(buffer, NIM_CONFIRM) == 0) {
                        // Play the game.  You are the 'O' player
                        int winner = playNIM(s, (char *) playerName.c_str(), (char *) host.c_str(),
                                             (char *) port.c_str(), SERVER);
                        finished = true;
                    }
                } else {
                    strcpy_s(responseStr, NIM_DECLINE);
                    UDP_send(s, responseStr, strlen(responseStr) + 1, (char *) host.c_str(), (char *) port.c_str());
                }


            }


        }

        if (!finished) {
            char previousBuffer[MAX_RECV_BUF];
            strcpy_s(previousBuffer, buffer);
            std::string previousHost;
            previousHost = host;
            std::string previousPort;
            previousPort = port;

            // Check for duplicate datagrams (can happen if broadcast enters from multiple ethernet connections)
            bool newDatagram = false;
            int status = wait(s, 1, 0);    // We'll wait a second to see if we receive another datagram
            while (!newDatagram && status > 0) {
                len = UDP_recv(s, buffer, MAX_RECV_BUF, (char *) host.c_str(), (char *) port.c_str());
                std::cout << timestamp() << " - Received: " << buffer << std::endl;
                if (strcmp(buffer, previousBuffer) == 0 &&
                    // If this datagram is identical to previous one, ignore it.
                    host == previousHost &&
                    port == previousPort) {
                    status = wait(s, 1, 0);            // Wait another second (still more copies?)
                } else {
                    newDatagram = true;        // if not identical to previous one, keep it!
                }
            }

            // If we waited one (or more seconds) and received no new datagrams, wait for one now.
            if (!newDatagram) {
                len = UDP_recv(s, buffer, MAX_RECV_BUF, (char *) host.c_str(), (char *) port.c_str());
                std::cout << timestamp() << " - Received: " << buffer << std::endl;
            }
        }
    }
    closesocket(s);

    return 0;
}