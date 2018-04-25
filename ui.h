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

//Prompts what kind of game they want to play
//RETURNS char that tells whether the user wants to be
//	a server, a client, or to quit
char chooseGameMode();

//Prompts for input and returns said input
std::string promptForInput(std::string prompt);

//Outputs the board
void showBoard(std::vector<int> board);

//Outputs messages given a name
void outputMessage(std::string name, std::string message);

