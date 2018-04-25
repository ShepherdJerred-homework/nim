#include "ui.h"
#include "nim.h"

using std::string;

void displayChatPrompt() {
    cout << "Type a message to send\n";
}

void displayOpponentChat(char message[]) {
    cout << "Opponent: " << message;
}

string getChatInput() {
    char c[MAX_CHAT_SIZE + 1];
    cin.ignore();
    cin.getline(c, MAX_CHAT_SIZE);
    string s = c;
    return s;
}

ACTION_TYPE_NS::ACTION_TYPE getActionType() {
    string input;
    cin >> input;
    if (input == "M" || input == "m") {
        return ACTION_TYPE_NS::MOVE;
    } else if (input == "C" || input == "c") {
        return ACTION_TYPE_NS::CHAT;
    } else if (input == "F" || input == "f") {
        return ACTION_TYPE_NS::FORFEIT;
    } else {
        return ACTION_TYPE_NS::INVALID;
    }
}

void displayActionPrompt() {
    cout << "What do you want to do?\n";
    cout << "(M)ove, (C)hat, (F)orfeit\n";
}

ACTION_TYPE_NS::ACTION_TYPE getActionFromUser() {
    ACTION_TYPE_NS::ACTION_TYPE actionType;

    do {
        displayActionPrompt();
        actionType = getActionType();
    } while (actionType == ACTION_TYPE_NS::ACTION_TYPE::INVALID);

    return actionType;
}

void clearScreen() {
    system("cls");
}

//Outputs the board
void showBoard(std::vector<int> board) {
    clearScreen();
    cout << "\nNIM Board:" << endl;
    cout << string(60, '-') << endl;
    short pileCount = 0;

    for (int i = 0; i < board.size(); i++) {
        std::cout << "Pile " << i + 1 << ": ";
        for (int j = 0; j < board[i]; j++) {
            cout << "* ";
            pileCount++;
        }
        cout << setw(46 - pileCount * 2) << "(" << setfill(' ') << setw(2) << pileCount << ") <-";
        cout << endl;
        pileCount = 0;
    }
    cout << string(60, '-') << endl << endl;
}

//Outputs messages given a name
void outputMessage(std::string name, std::string message) {
    cout << "Chat:" << endl << name << ": " << message << endl;
}