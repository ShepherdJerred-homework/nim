// playNIM.cpp
// contains functions necissary to play NIM after a UDP negotiation has taken place

#include "ui.h"
#include "NIM.h"
#include <iostream>
#include <winsock2.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

struct MoveStruct {
    int pileNumber;
    int numberOfRocks;
};

namespace TURN_STATUS_NS {
    enum TURN_STATUS {
        PENDING,
        TIMED_OUT,
        SUCCESS,
        FORFEIT
    };
}

class ConnectionInfo {
public:
    string host;
    string port;

    const char *hostAsCString() {
        return this->host.c_str();
    }

    const char *portAsCString() {
        return this->port.c_str();
    }
};

MoveStruct getMove(const std::vector<int> board) {
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

std::vector<int> createBoard(SOCKET s, std::string remoteIP, std::string remotePort) {
    //have the server pick the dimensions of the board to send to client
    std::vector<int> board;
    char dataStr[MAX_SEND_BUF];
    int numPiles = 0, pileSize = 0;
    while (numPiles < 3 || numPiles > 9) {
        std::cout << "How many piles would you like there to be?" << std::endl << "(Must be between 3 and 9)"
                  << std::endl;
        std::cin >> numPiles;
    }

    std::cout << "Please pick how many rocks there will be in each pile." << std::endl << "(Must be between 1 and 20)"
              << std::endl;

    for (int i = 0; i < numPiles; i++) {
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
    for (int i = 0; i < numPiles; i++) {
        std::string numRocks;
        if (board[i] < 10) {
            boardDimensions = boardDimensions + "0";
        }
        boardDimensions = boardDimensions + std::to_string(board[i]);
    }

    //change to c_string for transmission
    strcpy_s(dataStr, boardDimensions.c_str());

    UDP_send(s, dataStr, strlen(dataStr) + 1, (char *) remoteIP.c_str(), (char *) remotePort.c_str());

    return board;
}


std::vector<int> receiveBoard(SOCKET s, std::string remoteIP, std::string remotePort) {
    //receive the board from the opponent/client
    char boardRecvd[MAX_RECV_BUF];
    std::vector<int> board;
    UDP_recv(s, boardRecvd, MAX_RECV_BUF - 1, (char *) remoteIP.c_str(), (char *) remotePort.c_str());
    std::string boardData = boardRecvd;
    std::string extra = boardData.substr(0, 1);
    int numPiles = atoi(extra.c_str());

    for (int i = 1; i < boardData.length(); i += 2) {
        int pileSize;
        if (boardRecvd[i] == '0') {
            extra = boardData.substr(i + 1, 1);
            pileSize = atoi(extra.c_str());
        } else {
            extra = boardData.substr(i, 2);
            pileSize = atoi(extra.c_str());
        }
        board.push_back(pileSize);
    }
    return board;
}

void updateBoard(std::vector<int> &board, MoveStruct move) {
    if (move.pileNumber > 0 && move.pileNumber <= board.size()) {
        if (board[move.pileNumber - 1] >= move.numberOfRocks) {
            board[move.pileNumber - 1] -= move.numberOfRocks;
        }
    }
}

int checkForWin(std::vector<int> &board, bool myTurn, int localPlayer) {
    int winner;
    bool win = true;
    for (int i = 0; i < board.size(); i++) {
        if (board[i] > 0) {
            win = false;
        }
    }
    if (win) {
        if (localPlayer == SERVER && myTurn == true) {
            winner = SERVER;
        } else if (localPlayer == CLIENT && myTurn == false) {
            winner = SERVER;
        } else {
            winner = CLIENT;
        }

    } else {
        winner = NO_WINNER;
    }

    return winner;
}

void displayWinner(int winner, int localPlayerType) {
    if (winner == ABORT) {
        std::cout << timestamp() << " - No response from opponent.  Aborting the game..." << std::endl;
    } else if (winner == localPlayerType) {
        std::cout << "You WIN!" << std::endl;
    } else {
        std::cout << "I'm sorry.  You lost" << std::endl;
    }
}

void handleMove(vector<int> &board, ConnectionInfo remoteConnectionInfo, SOCKET socket) {
    char myMove[MAX_SEND_BUF];

    MoveStruct move = getMove(board);
    updateBoard(board, move);
    std::string moveStr = std::to_string(move.pileNumber);
    if (move.numberOfRocks <= 9) {
        moveStr = moveStr + "0" + std::to_string(move.numberOfRocks);
    } else {
        moveStr = moveStr + std::to_string(move.numberOfRocks);
    }
    strcpy_s(myMove, moveStr.c_str());

    UDP_send(socket, myMove, strlen(myMove), (char *) remoteConnectionInfo.hostAsCString(),
             (char *) remoteConnectionInfo.portAsCString());
    showBoard(board);
}

void handleOpponentMove(vector<int> &board, ConnectionInfo remoteConnectionInfo, SOCKET socket, char move[]) {
    cout << "waiting for opponent's move...\n";

    // TODO Need to take off the leading 'm' from the array
    std::string boardData = move;
    std::string extra = boardData.substr(0, 1);
    MoveStruct recvdMove;
    recvdMove.pileNumber = atoi(extra.c_str());
    if (recvdMove.pileNumber > 0 && recvdMove.pileNumber <= board.size()) {
        if (boardData[1] == '0') {
            extra = boardData.substr(2, 1);
            recvdMove.numberOfRocks = atoi(extra.c_str());
        } else {
            extra = boardData.substr(1, 2);
            recvdMove.numberOfRocks = atoi(extra.c_str());
        }
        if (recvdMove.numberOfRocks > 0 && recvdMove.numberOfRocks <= board[recvdMove.pileNumber - 1]) {
            updateBoard(board, recvdMove);
        } else {
            // TODO add code to automatically win
        }
    } else {
        // TODO add code to automatically win
    }
    showBoard(board);
}

void sendChat(string message, ConnectionInfo remoteConnectionInfo, SOCKET socket) {
    UDP_send(socket, const_cast<char *>(message.c_str()), strlen(message.c_str()),
             (char *) remoteConnectionInfo.hostAsCString(),
             (char *) remoteConnectionInfo.portAsCString());
}

void handleChatSend(ConnectionInfo remoteConnectionInfo, SOCKET socket) {
    displayChatPrompt();
    string chatInput = getChatInput();
    chatInput = "C" + chatInput + "\0";
    sendChat(chatInput, remoteConnectionInfo, socket);
}

void handleForfeitSend(ConnectionInfo remoteConnectionInfo, SOCKET socket) {

}

void handleMyTurn(vector<int> &board, ConnectionInfo remoteConnectionInfo, SOCKET socket) {
    ACTION_TYPE_NS::ACTION_TYPE actionType;

    do {
        actionType = getActionFromUser();

        switch (actionType) {
            case ACTION_TYPE_NS::MOVE:
                handleMove(board, remoteConnectionInfo, socket);
                break;
            case ACTION_TYPE_NS::CHAT:
                handleChatSend(remoteConnectionInfo, socket);
                break;
            case ACTION_TYPE_NS::FORFEIT:
                handleForfeitSend(remoteConnectionInfo, socket);
                break;
            case ACTION_TYPE_NS::INVALID:
                break;
        }
    } while (actionType != ACTION_TYPE_NS::MOVE);
}

void handleOpponentChat(char message[]) {
    displayOpponentChat(message);
}

TURN_STATUS_NS::TURN_STATUS handleOpponentTurn(vector<int> &board, ConnectionInfo remoteConnectionInfo, SOCKET socket) {
    char receiveBuffer[MAX_RECV_BUF];
    char opponentIP[v4AddressSize];
    strcpy_s(opponentIP, remoteConnectionInfo.hostAsCString());

    TURN_STATUS_NS::TURN_STATUS turnStatus = TURN_STATUS_NS::PENDING;

    do {
        int status = wait(socket, WAIT_TIME, 0);
        if (status > 0) {
            UDP_recv(socket, receiveBuffer, MAX_RECV_BUF - 1, opponentIP,
                     (char *) remoteConnectionInfo.portAsCString());
            if (strcmp(opponentIP, remoteConnectionInfo.hostAsCString()) == 0) {
                if (receiveBuffer[0] == 'C') {
                    handleOpponentChat(receiveBuffer);
                } else if (receiveBuffer[0] == 'm') {
                    handleOpponentMove(board, remoteConnectionInfo, socket, receiveBuffer);
                    turnStatus = TURN_STATUS_NS::SUCCESS;
                    break;
                } else if (receiveBuffer[0] == 'F') {
                    turnStatus = TURN_STATUS_NS::FORFEIT;
                    break;
                }
            }
        } else {
            turnStatus = TURN_STATUS_NS::TIMED_OUT;
        }
    } while (turnStatus == TURN_STATUS_NS::PENDING);

    return turnStatus;

}

void runGameLoop(std::vector<int> &board, int localPlayerType, ConnectionInfo remoteConnectionInfo, SOCKET socket) {
    int winner = NO_WINNER;

    bool isMyMove = localPlayerType == CLIENT;

    do {
        if (isMyMove) {
            handleMyTurn(board, remoteConnectionInfo, socket);
            isMyMove = false;
        } else {
            TURN_STATUS_NS::TURN_STATUS status = handleOpponentTurn(board, remoteConnectionInfo, socket);
            isMyMove = true;
            if (status == TURN_STATUS_NS::TIMED_OUT) {
                winner = ABORT;
            }
            if (status == TURN_STATUS_NS::FORFEIT) {
                winner = localPlayerType;
            }
        }
        if (winner == NO_WINNER) {
            winner = checkForWin(board, !isMyMove, localPlayerType);
        }
    } while (winner == NO_WINNER);
    displayWinner(winner, localPlayerType);
}

void
initializeBoard(std::vector<int> &board, SOCKET socket, ConnectionInfo remoteConnectionInfo, int localPlayerType) {
    if (localPlayerType == SERVER) {
        board = createBoard(socket, remoteConnectionInfo.host, remoteConnectionInfo.port);
    } else {
        board = receiveBoard(socket, remoteConnectionInfo.host, remoteConnectionInfo.port);
    }
}

int playNIM(SOCKET socket, string serverName, string remoteIP, string remotePort, int localPlayerType) {

    ConnectionInfo remoteConnectionInfo;
    remoteConnectionInfo.host = remoteIP;
    remoteConnectionInfo.port = remotePort;

    std::vector<int> board;

    initializeBoard(board, socket, remoteConnectionInfo, localPlayerType);
    showBoard(board);
    runGameLoop(board, localPlayerType, remoteConnectionInfo, socket);

    return -1;
}