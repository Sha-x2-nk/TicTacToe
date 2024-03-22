#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>

#include<iostream>

#define USER 'O' // we will be playing as O
#define AI 'X' // AI as X

char TTTBoard[3][3];

cv::Mat drawBoard() {
	cv::Mat board(300, 300, CV_8U, cv::Scalar(0));

	// drawing lines over the board
	// vertical lines. dividing screen in 3 parts.
	cv::line(board, cv::Point(99, 0), cv::Point(99, 299), cv::Scalar(255, 255, 255));
	cv::line(board, cv::Point(199, 0), cv::Point(199, 299), cv::Scalar(255, 255, 255));

	// horizontal lines. dividing screen in 3 parts.
	cv::line(board, cv::Point(0, 99), cv::Point(299, 99), cv::Scalar(255, 255, 255));
	cv::line(board, cv::Point(0, 199), cv::Point(299, 199), cv::Scalar(255, 255, 255));

	return board;
}

// only play if moves are left
bool areMovesLeft() {
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			if (TTTBoard[i][j] == '.') return true;
		}
	}
	return false;
}

// evaluate board
int evalBoard() {
	// return 1 if USER wins. 
	// return -1 if AI wins.

	// if there is a row of same letters.
	for (int row = 0; row < 3; ++row) {
		if (TTTBoard[row][0] == TTTBoard[row][1] && TTTBoard[row][1] == TTTBoard[row][2]) {
			if (TTTBoard[row][0] == USER) return 1;
			else if (TTTBoard[row][0] == AI) return -1;
		}
	}

	// col of same letters.
	for (int col = 0; col < 3; ++col) {
		if (TTTBoard[0][col] == TTTBoard[1][col] && TTTBoard[1][col] == TTTBoard[2][col]) {
			if (TTTBoard[0][col] == USER) return 1;
			else if (TTTBoard[0][col] == AI) return -1;
		}
	}

	// diagonals.
	if (TTTBoard[0][0] == TTTBoard[1][1] && TTTBoard[1][1] == TTTBoard[2][2]) {
		if (TTTBoard[0][0] == USER) return 1;
		else if (TTTBoard[0][0] == AI) return -1;
	}

	if (TTTBoard[0][2] == TTTBoard[1][1] && TTTBoard[1][1] == TTTBoard[2][0]) {
		if (TTTBoard[0][2] == USER) return 1;
		else if (TTTBoard[0][2] == AI) return -1;
	}

	return 0;
}

// now will come minMax algo. isMax = 1 means max, 0 means min 
int minimax(bool isMax) {
	int score = evalBoard();
	
	if (score != 0) {
		return score;
	}


	// tie
	if (areMovesLeft() == false)
		return 0;

	if (isMax) { // max player
		int best_val = -1000; // -INF. and we will now maximise it.

		// traverse full board and check all moves
		for (int r = 0; r < 3; ++r) {
			for (int c = 0; c < 3; ++c) {
				if (TTTBoard[r][c] == '.') {
					// make move.
					TTTBoard[r][c] = USER;

					int val = minimax(!isMax);
					best_val = std::max(best_val, val);

					// backtrack
					TTTBoard[r][c] = '.';
				}
			}
			return best_val;
		}
	}

	else { // min player
		int best_val = 1000; // INF. and we will now minimise it.

		// traverse full board and check all moves
		for (int r = 0; r < 3; ++r) {
			for (int c = 0; c < 3; ++c) {
				if (TTTBoard[r][c] == '.') {

					// make move
					TTTBoard[r][c] = AI;

					int val = minimax(!isMax);
					best_val = std::min(best_val, val);

					// backtrack
					TTTBoard[r][c] = '.';

				}
			}
			return best_val;
		}
	}
}

// creating AI move
void AIMove(cv::Mat board) {
	int best_score = 1000;
	int bestX = -1;
	int bestY = -1;
	
	for (int r = 0; r < 3; ++r) {
		for (int c = 0; c < 3; ++c) {
			if (TTTBoard[r][c] == '.') {
				// try kr yaha rakhne ka 
				TTTBoard[r][c] = AI;

				int score = minimax(false);

				if (score < best_score) {
					best_score = score;
					bestX = r;
					bestY = c;
				}

				// backtrack
				TTTBoard[r][c] = '.';
			}
		}
	}

	if (bestX != -1) {
		// print move
		TTTBoard[bestX][bestY] = AI;
		std::string txt;
		txt.push_back(AI);
		cv::putText(board, txt, cv::Point(bestX * 100, bestY * 100 + 100), cv::FONT_HERSHEY_COMPLEX, 3, cv::Scalar(255, 255, 255));
		cv::imshow("board", board);
	}
}

// mousecallback for drawing when user clicks in a box
void callBackFunc(int event, int x, int y, int flags, void* userdata) {
	cv::Mat board = static_cast<cv::Mat*>(userdata)[0];
	if (event == cv::EVENT_LBUTTONDOWN) {
		int rowIdx = x / 100; // 0, 1, 2
		int colIdx = y / 100; // 0, 1, 2

		if (TTTBoard[rowIdx][colIdx] == '.') {
			TTTBoard[rowIdx][colIdx] = USER;
			
			std::string txt; // "O" or "X"
			txt.push_back(USER);
			cv::putText(board, txt, cv::Point(rowIdx * 100, colIdx * 100 + 100), cv::FONT_HERSHEY_COMPLEX, 3, cv::Scalar(255, 255, 255));

			// terminate if won.
			cv::imshow("board", board);
			if (evalBoard() > 0) {
				printf("\nWON."); 
				return;
			}

			// waiting for AI
			AIMove(board);

			// terminate if lost.
			if (evalBoard() < 0) {
				printf("\nLOST.");
				return;
			}
		}
	}
}

int main() {
	// init board to have '.'
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			TTTBoard[i][j] = '.';
		}
	}

	// drawing board
	cv::Mat board = drawBoard();
	cv::namedWindow("board", cv::WINDOW_NORMAL);

	// to be called when user clicks on a screen section
	cv::setMouseCallback("board", callBackFunc, &board);
	cv::imshow("board", board);

	// terminate if key pressed.
	int k = cv::waitKey(0);
	if (k == 'q' || k == 'Q') {
		cv::destroyAllWindows();
	}

	return 0;
}