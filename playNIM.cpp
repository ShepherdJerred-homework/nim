//playNIM.cpp

//contains functions necissary to play NIM after a UDP negotiation has taken place
#include "ui.h"
#include "NIM.h"
#include <iostream>
#include <winsock2.h>
#include <string>
#include <vector>

std::vector<int> receiveBoard(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort);


struct MoveStruct {
	int pileNumber;
	int numberOfRocks;
};

MoveStruct getMove(const std::vector<int> board)
{
	MoveStruct move;
	do {

		std::cout << "Which pile would you like to take from? ";
		std::cin >> move.pileNumber;
	} while (move.pileNumber <= 0 || move.pileNumber > board.size());

	do {
		std::cout << "How many rocks would you like to take? ";
		std::cin >> move.numberOfRocks;
	} while (move.numberOfRocks <= 0 || move.numberOfRocks > board[move.pileNumber - 1]);

	return move;
}

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

	UDP_send(s, dataStr, strlen(dataStr) + 1, (char*)remoteIP.c_str(), (char*)remotePort.c_str());

	return board;
}


std::vector<int> receiveBoard(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort) {
	//receive the board from the opponent/client
	char boardRecvd[MAX_RECV_BUF];
	std::vector<int> board;
	UDP_recv(s, boardRecvd, MAX_RECV_BUF - 1, (char*)remoteIP.c_str(), (char*)remotePort.c_str());
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

void updateBoard(std::vector<int> &board, MoveStruct move)
{
	if (move.pileNumber > 0 && move.pileNumber <= board.size())
	{
		if (board[move.pileNumber - 1] >= move.numberOfRocks)
		{
			board[move.pileNumber - 1] -= move.numberOfRocks;
		}
	}
}

int check4Win(std::vector<int> board, bool myTurn, int localPlayer)
{
	int winner;
	bool win = true;
	for (int i = 0; i < board.size(); i++)
	{
		if (board[i] > 0)
		{
			win = false;
		}
	}
	if (win)
	{
		if (localPlayer == server && myTurn == true)
		{
			winner = server;
		}
		else if (localPlayer == client && myTurn == false)
		{
			winner = server;
		}
		else
		{
			winner = client;
		}

	}
	else
	{
		winner = noWinner;
	}

	return winner;
}


int playNIM(SOCKET s, std::string serverName, std::string remoteIP, std::string remotePort, int localPlayer)
{
	int winner = noWinner;
	char myMove[MAX_SEND_BUF];
	char opponentIP[v4AddressSize];
	char opponentMove[MAX_RECV_BUF];
	std::vector<int> board;
	bool isMyMove;
	int opponent;

	if (localPlayer == client) {
		opponent = server;
		isMyMove = true;
	}
	else {
		opponent = client;
		isMyMove = false;
	}

	if (localPlayer == server)
		board = createBoard(s, serverName, remoteIP, remotePort);
	else
		board = receiveBoard(s, serverName, remoteIP, remotePort);
	showBoard(board);

	/*
	* This causes issue, haven't debugged yet. Feel free to take a look.
	*/
	while (winner == noWinner) {
		if (isMyMove) {

			MoveStruct move = getMove(board);
			updateBoard(board, move);
			std::string moveStr = std::to_string(move.pileNumber);
			if (move.numberOfRocks <= 9)
			{
				moveStr = moveStr + "0" + std::to_string(move.numberOfRocks);
			}
			else
			{
				moveStr = moveStr + std::to_string(move.numberOfRocks);
			}
			strcpy_s(myMove, moveStr.c_str());

			UDP_send(s, myMove, strlen(myMove) , (char*)remoteIP.c_str(), (char*)remotePort.c_str());
			showBoard(board);
			isMyMove = false;
		}
		else {
			cout << "waiting for opponent's move...\n";
			int status = wait(s, WAIT_TIME, 0);
			if (status > 0)
			{
				strcpy_s(opponentIP, remoteIP.c_str());
				UDP_recv(s, opponentMove, MAX_RECV_BUF - 1, opponentIP, (char*)remotePort.c_str());
				if (strcmp(opponentIP, remoteIP.c_str()) == 0) {
					//checkCommandType(opponentMove, )

					// if we are playing against our opponent, process their move
					std::string boardData = opponentMove;
					std::string extra = boardData.substr(0, 1);
					MoveStruct recvdMove;
					recvdMove.pileNumber = atoi(extra.c_str());
					if (recvdMove.pileNumber > 0 && recvdMove.pileNumber <= board.size())
					{
						if (boardData[1] == '0')
						{
							extra = boardData.substr(2, 1);
							recvdMove.numberOfRocks = atoi(extra.c_str());
						}
						else
						{
							extra = boardData.substr(1, 2);
							recvdMove.numberOfRocks = atoi(extra.c_str());
						}
						if (recvdMove.numberOfRocks > 0 && recvdMove.numberOfRocks <= board[recvdMove.pileNumber - 1])
						{
							updateBoard(board, recvdMove);
						}
						else
						{
							//add code to automatically win
						}
					}
					else
					{
						//add code to automatically win
					}
					showBoard(board);
					isMyMove = true;
				}
			}
			else
			{
				winner = ABORT;
			}
		}



		if (winner == ABORT) {
			std::cout << timestamp() << " - No response from opponent.  Aborting the game..." << std::endl;
		}
		else {
			winner = check4Win(board, !isMyMove, localPlayer);
		}

		if (winner == localPlayer)
			std::cout << "You WIN!" << std::endl;

		else if (winner == opponent)
			std::cout << "I'm sorry.  You lost" << std::endl;
	}
	return -1;
}