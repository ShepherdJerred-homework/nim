#include "ui.h"

//Prompts what kind of game they want to play
//RETURNS char that tells whether the user wants to be
//	a server, a client, or to quit
char chooseGameMode() 
{
	char mode;
	cout << "choose game mode. c:client s:server q:quit" << endl << "Input: ";
	cin >> mode;
	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	return mode;
}

//Prompts for input and returns said input
std::string promptForInput(std::string prompt) 
{
	cout << prompt << endl << "Input: ";
	std::string input;
	getline(cin, input);

	return input;
}

//Outputs the board
void showBoard(std::vector<int> board) 
{
	cout << "\nNIM Board:" << endl;
	cout << string(60, '-') << endl;
	short pileCount = 0;

	for (int i = 0; i < board.size(); i++)
	{
		std::cout << "Pile " << i + 1 << ": ";
		for (int j = 0; j < board[i]; j++)
		{
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
void outputMessage(std::string name, std::string message) 
{
	cout<< "Chat:" << endl << name << ": " << message << endl;
}