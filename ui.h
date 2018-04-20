#pragma once
#include "stdafx.h"
#include <string>
#include <vector>
#include <iostream>
using std::cout;
using std::endl;
using std::cin;
using std::getline;

//Prompts what kind of game they want to play
//RETURNS char that tells whether the user wants to be
//	a server, a client, or to quit
char chooseGameMode();

//Prompts for input and returns said input
std::string promptForInput(std::string prompt);

//Outputs the board
void showBoard(std::vector<int> board, int numPiles);

//Outputs messages given a name
void outputMessage(std::string name, std::string message);

