//playNIM.cpp

//contains functions necissary to play NIM after a UDP negotiation has taken place
#include "ui.h"
#include "NIM.h"
#include <iostream>
#include <winsock2.h>
#include <string>
#include <vector>

std::vector<int> receiveBoard(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort);

std::vector<int> createBoard(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort)
{
	//have the server pick the dimensions of the board to send to client
	std::vector<int> board;
	char dataStr[MAX_SEND_BUF];
	int numPiles = 0, pileSize = 0;
	while (numPiles < 3 || numPiles > 9) {
		std::cout << "How many piles would you like there to be?" << std::endl << "(Must be between 3 and 9)" << std::endl;
		std::cin >> numPiles;
	}

	std::cout << "Please pick how many rocks there will be in each pile." << std::endl << "(Must be between 1 and 20)" << std::endl;

	for (int i = 0; i < numPiles; i++)
	{
		std::cout << "Pile " << i + 1 << ": ";
		std::cin >> pileSize;
		while (pileSize < 1 || pileSize > 20) {
			std::cout << "(Must be between 1 and 20)" << std::endl;
			std::cin >> pileSize;
		}
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

	UDP_send(s, dataStr, MAX_SEND_BUF + 1, (char*)remoteIP.c_str(), (char*)remotePort.c_str());

	return board;
}


std::vector<int> receiveBoard(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort) {
	//receive the board from the opponent/client
	char boardRecvd[MAX_RECV_BUF];
	std::vector<int> board;
	UDP_recv(s, boardRecvd, MAX_RECV_BUF, (char*)remoteIP.c_str(), (char*)remotePort.c_str());
	std::string boardData = boardRecvd;
	std::string extra = boardData.substr(0, 1);
	int numPiles = atoi(extra.c_str());

	for (int i = 1; i < boardData.length(); i += 2)
	{
		int pileSize;
		if (boardRecvd[i] == '0')
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
	return board;
}


int playNIM(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort, int localPlayer)
{
	char myMove[MAX_SEND_BUF];
	char opponentMove[MAX_RECV_BUF];
	std::vector<int> board;
	bool isMyMove = localPlayer % 2; // if local player is playing as server (2) 
									// this evaluates to false b/c client always has first move

	if (localPlayer == server)
		board = createBoard(s, serverName, remoteIP, remotePort);
	else
		board = receiveBoard(s, serverName, remoteIP, remotePort);
	showBoard(board);

	/*
	* This causes issue, haven't debugged yet. Feel free to take a look.
	*/
	while (1) {
		if (isMyMove) {
			//getMove(myMove);
			UDP_send(s, myMove, MAX_SEND_BUF, (char*)remoteIP.c_str(), (char*)remotePort.c_str());
		}
		else {
			cout << "waiting for opponent's move...\n";
			UDP_recv(s, opponentMove, MAX_RECV_BUF, (char*)remoteIP.c_str(), (char*)remotePort.c_str());
		}

		showBoard(board);
		isMyMove = !isMyMove;
	}
	return -1;
}