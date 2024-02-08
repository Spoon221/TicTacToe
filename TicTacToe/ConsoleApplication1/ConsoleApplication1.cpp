#include <iostream>
#include "TicTacToe.h"

using namespace std;

int main() {
	TicTacToe game;

	string start;

	/* Служит для разнообразия начала игры
	cout << "Who starts: ";
	cin >> start;

	game.isFirstMove = !game.humanTurn; Начинает пк

	game.humanTurn = start != game.aiSymbol; Начинает игрок
	*/
	game.humanTurn = start != game.aiSymbol;
	
	game.playGame();

	string res;
	cout << endl << "Play again (y/n): ";
	cin >> res;
	if (res == "y")
		main();

	return 0;
}