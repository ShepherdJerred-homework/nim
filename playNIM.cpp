//playNIM.cpp

//contains functions necissary to play NIM after a UDP negotiation has taken place
#include "ui.h"
#include "NIM.h"
#include <iostream>
#include <winsock2.h>
#include <string>
#include <vector>

std::vector<int> createBoard(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort, int localPlayer)
{
	std::vector<int> board;
	char dataStr[MAX_SEND_BUF];
	if (localPlayer == server)
	{

		//have the player pick the dimensions of the board

		std::cout << "How many piles would you like there to be?" << std::endl << "(Must be between 3 and 9)" << std::endl;
		int numPiles;
		std::cin >> numPiles;
		std::cout << "Please pick how many rocks there will be in each pile." << std::endl << "(Must be between 1 and 20)" << std::endl;
		
		for (int i = 0; i < numPiles; i++)
		{
			std::cout << "Pile " << i + 1 << ": ";
			int pileSize;
			std::cin >> pileSize;
			board.push_back(pileSize);


		}
		std::cout << std::endl;
		std::cout << "Sending board to opponent..." << std::endl;

		std::string boardDimensions = std::to_string(numPiles);
		for (int i = 0; i < numPiles; i++)
		{
			std::string numRocks;
			if (board[i] < 10)
			{
				boardDimensions = boardDimensions + "0";

			}
			boardDimensions = boardDimensions + std::to_string(board[i]);

		}
		//change to c_string for transmission
		strcpy_s(dataStr, boardDimensions.c_str());

		UDP_send(s, dataStr, strlen(dataStr) + 1, (char*)remoteIP.c_str(), (char*)remotePort.c_str());
	}

	else
	{
		//wait to receve the dimensions of the board 
		int len = UDP_recv(s, dataStr, MAX_RECV_BUF, (char*)remoteIP.c_str(), (char*)remotePort.c_str());

		//parse the data string to make the board
		std::string boardData = dataStr;
		std::string extra = boardData.substr(0, 1);
		int numPiles = atoi(extra.c_str());
		
		board;
		for (int i = 1; i < boardData.length(); i += 2)
		{
			int pileSize;
			if (dataStr[i] == '0')
			{
				extra = boardData.substr(i + 1, 1);
				pileSize = atoi(extra.c_str());
				
			}
			else
			{
				
				extra = boardData.substr(i, 2);
				pileSize = atoi(extra.c_str());


			}
			board.push_back(pileSize);
		}
	}

		return board;
}


int playNIM(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort, int localPlayer)
{
	std::cout << std::endl << "you made it this far" << std::endl << std::endl;
	char dataStr[MAX_SEND_BUF];


	std::vector<int> board = createBoard(s, serverName, remoteIP, remotePort, localPlayer);

	showBoard(board);


		
	
	
	return -1;
}