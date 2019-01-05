#pragma once

#include <iostream>

#include <vector>
#include <stack>
#include <string>
#include <fstream>
#include <memory>

#include <algorithm>

#include <thread>
#include <mutex>


#ifdef _WINDOWS

#define _WIN32_WINNT  0x0600
#include "Windows.h"

#endif


using namespace std;

typedef vector<int> VINT;

typedef vector < VINT>  VCOLLECTION;


typedef struct {
	int queenId;			//index starts from 0 onwards
	int currentIndexPos;	//Queen placed at position
	int availableIndexesCount; // Total available moves on board for this queen
	VINT availableIndexes;// indexes as value for this queen possible to place in reference to the previous queen, -1 = already traversed/placed
	VINT vBoardStateBeforePlacingQueen; // Array of x*y 
	int _queenMovedCount;  // queed index id moved counter, queen can only move as many places available to it.
}QueenPlacedAt;

class NewQueenSolution
{
	int _numCores; // total hardware threads forced / designated to
	int x_length;  // total horizontal space X should be equal to Y
	int y_length;  // total horizontal space Y should be equal to X
	unsigned int _firstQueenMovedPos ; //exit measuring variable for this queen / thread execution
	long _solutionCounter; //local solution counter to find the current queen / thread placment has how many solutions.
	ofstream _ofileSolution ; // Dumping information in output solution file.
	

	static unsigned int BOARD_TO_ARRAY_LEN;

public:

	static VCOLLECTION vSolutions;

	NewQueenSolution()=delete; // Dont need default initialization
	NewQueenSolution(int);
	NewQueenSolution(int num_threads, int x, int y);
	~NewQueenSolution();
	

	VINT GetFreshBoard();

	bool AddUniqueSolutionOnly(VINT & vt); // Find if this solution already added in list, only add if unique and new
	void StartFromFreshBoard();
	void StartFromFreshBoard_Thread(int position); // Entry point for the Threaded solution where first queen will be placed at "position"

	void PrintStackInfo(stack<std::shared_ptr<QueenPlacedAt>> vstemp);
	void PrintSolutions(VCOLLECTION &vSln); // Print all solution collection vector

	int geFirstEmptyPosition(VINT& v); //return first index of the chess[8x8] array[0-63] that has value 0=unoccupied/safe
	int geLastEmptyPosition(VINT& v); //return last index of the chess[8x8] array[0-63] that has value 0=unoccupied/safe

	VINT getAllAvailableIndex(VINT& v);
	int getNextEmptyPosition(VINT & v, int pos); //return next index of the chess[8x8]=>array[0-63] that has value 0=unoccupied/safe after/from arg "pos"
	void mark_unavailable_indexes(VINT& v, int pos); //return a board/array[0-63] state after placing queen at position
	
	int SaveQueenOnStackPush(VINT& v, stack <std::shared_ptr<QueenPlacedAt>>& vs); // unused but left for reference
	int SaveQueenOnStackPush_Thread(VINT& v, stack <std::shared_ptr<QueenPlacedAt>>& vs, int position );

	int RearrangeLastQueen(VINT & v, stack<std::shared_ptr<QueenPlacedAt>>& vs); // Most vital function, take a step back and place last the queen at next available position
	int UpdateTopOfStackQueen(VINT& v, stack <std::shared_ptr<QueenPlacedAt>>& vs); //Recent Queen state update

private:	
	int convert_2d_to_index(int xpos, int ypos);
	void mark_horizontal(VINT&, int index);
	
	void mark_veritcal(VINT&, int index);
	void mark_diagonals(VINT&, int index);
};

