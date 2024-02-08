#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <chrono>
#include <thread>
#include <map>

using namespace std;
using namespace chrono;
using namespace this_thread;

int boardDimension = 3;
int delay = 1500;
enum States { LOST = -1, TIE, WON };

class TicTacToe {
public:
	TicTacToe() : gameBoard(boardDimension, vector<string>(boardDimension, " ")), gameResult(TIE) {}

	void playGame() 
	{
		printBoard();

		do 
		{
			string position;

			if (humanTurn) 
			{
				cout << endl << endl << "Place an " << humanSymbol << " to: ";
				cin >> position;

				if (!ValidPosition(position)) 
				{
					cout << "Invalid position!" << endl;
					sleep_for(milliseconds(delay));
					continue;
				}
			}
			else if (isFirstMove) 
			{
				position = RandomAIPosition();
				isFirstMove = false;
			}
			else 
				position = AIMove();

			MakeMove(position, (humanTurn ? humanSymbol : aiSymbol));
			humanTurn = !humanTurn;
			system("cls");
			printBoard();
		} 
		while (!GameOver());

		switch (gameResult) 
		{
		case LOST:
			cout << endl << "You lost!" << endl;
			break;
		case TIE:
			cout << endl << "Tie!" << endl;
			break;
		case WON:
			cout << endl << "You won!" << endl;
			break;
		default:
			break;
		}
	}

	bool humanTurn;
	bool isFirstMove;
	string humanSymbol = "O";
	string aiSymbol = "X";

private:
	vector<vector<string>> gameBoard;
	map<string, int> memo;
	States gameResult;

	void printBoard() const 
	{
		cout << "   1   2   3" << endl;
		for (int i = 0; i < boardDimension; i++) 
		{
			cout << " " << i + 1;
			for (int j = 0; j < boardDimension; j++)
				cout << " " << gameBoard[i][j] << (j != boardDimension - 1 ? " |" : "");
			cout << endl << (i != boardDimension - 1 ? "   -----------" : "") << endl;
		}
	}

	bool GameOver() 
	{
		if (gameBoard[1][1] != " " 
			&& ((gameBoard[0][0] == gameBoard[1][1] && gameBoard[1][1] == gameBoard[2][2]) 
			|| (gameBoard[0][2] == gameBoard[1][1] && gameBoard[1][1] == gameBoard[2][0]))) 
		{
			gameResult = (gameBoard[1][1] == humanSymbol ? WON : (gameBoard[1][1] == aiSymbol ? LOST : TIE));
			return true;
		}

		for (int i = 0; i < boardDimension; i++) 
		{
			if (gameBoard[i][0] != " " && gameBoard[i][0] == gameBoard[i][1] && gameBoard[i][0] == gameBoard[i][2]) 
			{
				gameResult = (gameBoard[i][0] == humanSymbol ? WON : (gameBoard[i][0] == aiSymbol ? LOST : TIE));
				return true;
			}

			if (gameBoard[0][i] != " " && gameBoard[0][i] == gameBoard[1][i] && gameBoard[0][i] == gameBoard[2][i]) 
			{
				gameResult = (gameBoard[0][i] == humanSymbol ? WON : (gameBoard[0][i] == aiSymbol ? LOST : TIE));
				return true;
			}
		}

		for (int i = 0; i < boardDimension; i++)
			for (int j = 0; j < boardDimension; j++)
				if (gameBoard[i][j] == " ")
					return false;

		gameResult = TIE;
		return true;
	}

	bool ValidPosition(const string& pos) 
	{
		if (pos.length() != 2 
			|| pos[0] < '1' 
			|| pos[0] > '3' 
			|| pos[1] < '1' 
			|| pos[1] > '3' 
			|| gameBoard[pos[0] - '1'][pos[1] - '1'] != " ")
			return false;
		return true;
	}

	void MakeMove(const string& pos, const string& player) 
	{
		gameBoard[pos[0] - '1'][pos[1] - '1'] = player;
	}

	string IntPosString(int i, int j) 
	{
		return to_string(i + 1) + to_string(j + 1);
	}

	string GetBoardSignature() 
	{
		string sign;
		for (int i = 0; i < boardDimension; i++)
			for (int j = 0; j < boardDimension; j++)
				sign += gameBoard[i][j];

		return sign;
	}

	string RandomAIPosition() {
		static random_device rd;
		static mt19937 mt(rd());
		static uniform_int_distribution<int> dist(0, boardDimension - 1);

		vector<pair<int, int>> emptyCells;
		for (int i = 0; i < boardDimension; i++) 
		{
			for (int j = 0; j < boardDimension; j++) 
			{
				if (gameBoard[i][j] == " ")
					emptyCells.push_back({ i, j });
			}
		}

		if (emptyCells.empty()) 
			return "-1 -1"; 

		int randomIndex = dist(mt) % emptyCells.size();
		return IntPosString(emptyCells[randomIndex].first, emptyCells[randomIndex].second);
	}


	string AIMove() 
	{
		static random_device rd;
		static mt19937 mt(rd());
		static uniform_int_distribution<int> dist(0, boardDimension - 1);

		int bestVal = INT_MIN;
		string bestMove = "";

		vector<pair<int, int>> emptyCells;
		for (int i = 0; i < boardDimension; i++) 
		{
			for (int j = 0; j < boardDimension; j++) 
			{
				if (gameBoard[i][j] == " ")
					emptyCells.push_back({ i, j });
			}
		}

		for (const auto& cell : emptyCells) 
		{
			gameBoard[cell.first][cell.second] = aiSymbol;
			int moveVal = MoveBestAI(0, false, INT_MIN, INT_MAX);
			gameBoard[cell.first][cell.second] = " ";

			if (moveVal > bestVal) 
			{
				bestMove = IntPosString(cell.first, cell.second);
				bestVal = moveVal;
			}
		}

		return bestMove;
	}


	int MoveBestAI(int depth, bool isMax, int alpha, int beta) 
	{
		string key = GetBoardSignature();
		if (memo.count(key)) 
			return memo[key];

		if (GameOver())
			return (gameResult == WON ? -10 + depth : (gameResult == LOST ? 10 - depth : 0));

		int best = (isMax ? INT_MIN : INT_MAX);
		for (int i = 0; i < boardDimension; i++) 
		{
			for (int j = 0; j < boardDimension; j++) 
			{
				if (gameBoard[i][j] == " ") 
				{
					gameBoard[i][j] = (isMax ? aiSymbol : humanSymbol);
					int val = MoveBestAI(depth + 1, !isMax, alpha, beta);
					gameBoard[i][j] = " ";

					if (isMax) 
					{
						best = max(best, val);
						alpha = max(alpha, val);
					}
					else 
					{
						best = min(best, val);
						beta = min(beta, val);
					}

					if (beta <= alpha)
						break;
				}
			}
		}
		memo[key] = best;
		return best;
	}
};