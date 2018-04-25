#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <string>
#include <iomanip>

using std::cout;
using std::endl;
using std::cin;
using std::getline;
using std::string;
using std::setw;
using std::setfill;


namespace ACTION_TYPE_NS {
    enum ACTION_TYPE {
        MOVE, CHAT, FORFEIT, INVALID
    };
}

void displayOpponentChat(char *message);

void displayChatPrompt();

string getChatInput();

ACTION_TYPE_NS::ACTION_TYPE getActionFromUser();

// Outputs the board
void showBoard(std::vector<int> board);

// Outputs messages given a name
void outputMessage(std::string name, std::string message);

