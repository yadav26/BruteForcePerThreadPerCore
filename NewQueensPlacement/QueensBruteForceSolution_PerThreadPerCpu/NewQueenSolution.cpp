#include "NewQueenSolution.h"


std::mutex g_i_mutex; // lock the global static solution vector update by multiple threads
std::mutex g_i_mutexRead;

VCOLLECTION NewQueenSolution::vSolutions = {};
 unsigned int NewQueenSolution::BOARD_TO_ARRAY_LEN = 0;

NewQueenSolution::NewQueenSolution(int num_threads)
{
	x_length = 8;
	y_length = 8;
	BOARD_TO_ARRAY_LEN = x_length * y_length;

	_firstQueenMovedPos = 0;
	_solutionCounter = 0;
	_numCores = num_threads;
	
	_ofileSolution.open("8QueenSolutions.txt", std::ofstream::out | std::ofstream::app);
	//_ofileSolution << "\n\n==============NEW RUN====================\n\n";
	_ofileSolution.close();
}

NewQueenSolution::NewQueenSolution(int num_threads, int x, int y)
{
	x_length = x;
	y_length = y;
	BOARD_TO_ARRAY_LEN = x_length * y_length;

	_firstQueenMovedPos = 0;
	_solutionCounter = 0;
	_numCores = num_threads;

	_ofileSolution.open("8QueenSolutions.txt", std::ofstream::out | std::ofstream::app);
	//_ofileSolution << "\n\n==============NEW RUN====================\n\n";
	_ofileSolution.close();
}

NewQueenSolution::~NewQueenSolution()
{
}

VINT NewQueenSolution::GetFreshBoard()
{
	vector<int> v (BOARD_TO_ARRAY_LEN,0);
	return std::move(v);
}

int NewQueenSolution::geFirstEmptyPosition(VINT& v)
{
	VINT::iterator vit;
	vit = std::find(v.begin(), v.end(), 0);
	if (vit == v.end())
		return -1;
	
	return std::distance(v.begin(), vit);

}

int NewQueenSolution::geLastEmptyPosition(VINT& v)
{
	
	auto vit = std::find(v.rbegin(), v.rend(), 0);

	if (vit == v.rend())
		return -1;

	auto n = std::distance(vit, v.rend()) ;
	//std::cout << "Element 4 found at position v[" << n << "].\n";

	return n;

}

int NewQueenSolution::getNextEmptyPosition(VINT& v, int currentIndexPos)
{
	VINT::iterator vit;

		if (v.begin() + currentIndexPos + 1 == v.end())
		{
			return -1;
		}
		else 
			vit = v.begin() + currentIndexPos + 1;

	
	VINT vtemp(vit, v.end());
	
	

	vit = std::find(vtemp.begin(), vtemp.end(), 0);
	if (vit == vtemp.end())
		return -1;

	int d = std::distance(vtemp.begin(), vit);
	
	return d + currentIndexPos + 1;

}

VINT NewQueenSolution::getAllAvailableIndex(VINT& v)
{
	VINT vt;
	
	int IndexCounter = 0;
	for (auto itr = v.begin(); itr != v.end(); ++itr)
	{
		if(*itr == 0 )
			vt.push_back(IndexCounter);

		++IndexCounter;
	}
	return move(vt);
}

int NewQueenSolution::UpdateTopOfStackQueen(VINT& v, stack <std::shared_ptr<QueenPlacedAt>>& vs)
{
	auto top = vs.top();

	if (top->availableIndexesCount > 0)
	{
		++top->_queenMovedCount; // increment queenMoved one step

		top->currentIndexPos = top->availableIndexes.at(top->_queenMovedCount - 1); // to find index starting from 0

		--top->availableIndexesCount; // decrement can be moved count

		v = top->vBoardStateBeforePlacingQueen;

		v[top->currentIndexPos] = 1; //return board with queen placed at currentIndexPos
		//cout << "\n Updating Queen [" << vs.size() << "], can be moved = [" << top->availableIndexesCount<< "]\n";
	}

	return top->currentIndexPos;
}

int NewQueenSolution::SaveQueenOnStackPush(VINT& v, stack <std::shared_ptr<QueenPlacedAt>>& vs )
{
	shared_ptr <QueenPlacedAt> shPtr(new QueenPlacedAt());
	
	shPtr->queenId = vs.size();

	shPtr->vBoardStateBeforePlacingQueen = v;
	
	shPtr->availableIndexes = getAllAvailableIndex(v); //array of indexes queen can be placed at

	shPtr->_queenMovedCount = 1; // now place the first move

 
	shPtr->currentIndexPos = shPtr->availableIndexes.at(shPtr->_queenMovedCount -1); // get index where queen placed

	v[shPtr->currentIndexPos] = 1; //return the board with new queen placed.

	shPtr->availableIndexesCount = shPtr->availableIndexes.size() -1; // Queen can be placed that many places and this one already placed

	vs.push(shPtr); 


	//cout << "\n Stored Board state. VS=["<< vs.size()<<"], Queen pos = [" << shPtr->currentIndexPos << "].";

	return shPtr->currentIndexPos;
}

int NewQueenSolution::SaveQueenOnStackPush_Thread(VINT& v, stack <std::shared_ptr<QueenPlacedAt>>& vs, int position )
{
	shared_ptr <QueenPlacedAt> shPtr(new QueenPlacedAt());
	
	shPtr->queenId = vs.size();

	shPtr->vBoardStateBeforePlacingQueen = v;
	
	shPtr->availableIndexes = getAllAvailableIndex(v); //array of indexes queen can be placed at

	shPtr->_queenMovedCount = 1; // now place the first move

	if(shPtr->queenId  == 0) 
		shPtr->currentIndexPos = position;
	else 
		shPtr->currentIndexPos = shPtr->availableIndexes.at(shPtr->_queenMovedCount -1); // get index where queen placed

	v[shPtr->currentIndexPos] = 1; //return the board with new queen placed.

	shPtr->availableIndexesCount = shPtr->availableIndexes.size() -1; // Queen can be placed that many places and this one already placed

	vs.push(shPtr); 


	//cout << "\n Stored Board state. VS=["<< vs.size()<<"], Queen pos = [" << shPtr->currentIndexPos << "].";

	return shPtr->currentIndexPos;
}

int NewQueenSolution::RearrangeLastQueen(VINT& v, stack <std::shared_ptr<QueenPlacedAt>>& vs )
{
	int currPos = -1;
	while (!vs.empty())
	{
		auto vit = vs.top();
		v = vit->vBoardStateBeforePlacingQueen;

		if (vit->availableIndexesCount == 0)
			vs.pop();
		else
		{
			currPos = UpdateTopOfStackQueen(v, vs);
			mark_unavailable_indexes(v, currPos);
			break;
		}
		if (vs.size() == 1)
			_firstQueenMovedPos = vit->_queenMovedCount;

	}
	return currPos;
}

bool NewQueenSolution::AddUniqueSolutionOnly( VINT& vt)
{
	if(vt.empty())
		return true;

	for(auto vitr = vSolutions.rbegin(); vitr != vSolutions.rend(); ++vitr )
	{
		int counter = 0;
		for(auto& val: *vitr)
		{
			if (val != vt[counter])
				break;
	
			++counter;
		}
		if (counter == x_length)
			return false;
	}

	return true;
}

void NewQueenSolution::StartFromFreshBoard_Thread(int position)
{
	//cout << "Entering StartFromFreshBoard_Thread.. .First Queen placed at = [ " << position << " ]\n";
	
	stack <std::shared_ptr<QueenPlacedAt>> vs;

	VINT v = GetFreshBoard();

	int currPos = position;	

	while (_firstQueenMovedPos < 2)
	{
		do
		{
			auto vit = std::find(v.begin(), v.end(), 0);
			if (vit == v.end())
				break;

			currPos = SaveQueenOnStackPush_Thread(v, vs, position);
			mark_unavailable_indexes(v, currPos);
		} while (currPos != -1);


		if (vs.size() == x_length ) // consider x_length is maximum queens can be placed
		{
			VINT vtemp;

//			if(_solutionCounter % 100 == 0 )
//				cout << "\n\n****found a solution ***** Thread #" 
//					 << position 
//					 << ":: on CPU " 
//#ifdef _WINDOWS
//					<< GetCurrentProcessorNumber()
//#elif _NON_WINDOWS
//					 << sched_getcpu() 
//#endif
//					 << ": Soln # [" 
//					 << ++_solutionCounter 
//					 << " ]";

			stack <std::shared_ptr<QueenPlacedAt>> vstemp = vs;
			//PrintStackInfo(vs);
#ifdef DEBUG
			PrintStackInfo(vs);
#endif
			while (!vstemp.empty())
			{
				
				auto vit = vstemp.top();
				vtemp.push_back(vit->currentIndexPos);
				
				vstemp.pop();
			}

			sort(vtemp.begin(), vtemp.end());
			
			std::lock_guard<std::mutex> lock(g_i_mutex);

			if (true == AddUniqueSolutionOnly(vtemp))
			{
				vSolutions.push_back(vtemp);
				
				//lock following code
				

				int Queen = 0;
				_ofileSolution.open("8QueenSolutions.txt", std::ofstream::out | std::ofstream::app);
				_ofileSolution << "\nSoution #"<< vSolutions.size() << "--";

				for (auto &x : vtemp)
					_ofileSolution << x << ",";

				_ofileSolution.close();

			}
				
		}
		
		//cout << "\n\nNO more space to place the next queen vs=["<<vs.size()<<"], pop and rearrange **";
		currPos = RearrangeLastQueen(v, vs);

	}
	
	std::lock_guard<std::mutex> lock(g_i_mutex);
	cout << "\n******** Finished finding all solution = [ " << position << " ], Total Solution found = [ "<< vSolutions.size()<<" ]";

	//PrintSolutions(vSolutions);

	return;
}

void NewQueenSolution::StartFromFreshBoard()
{
	stack <std::shared_ptr<QueenPlacedAt>> vs;
	VINT v = GetFreshBoard();

	int currPos = 0;
	while (_firstQueenMovedPos <= BOARD_TO_ARRAY_LEN -1)
	{
		do
		{
			auto vit = std::find(v.begin(), v.end(), 0);
			if (vit == v.end())
				break;

			currPos = SaveQueenOnStackPush(v, vs);
			mark_unavailable_indexes(v, currPos);
		} while (currPos != -1);


		if (vs.size() == x_length )
		{
			cout << "\n\n****found a solution ***** , Counter = [ " << ++_solutionCounter << " ]";
			VINT vtemp;
			stack <std::shared_ptr<QueenPlacedAt>> vstemp = vs;
			//PrintStackInfo(vs);
#ifdef DEBUG
			PrintStackInfo(vs);
#endif
			while (!vstemp.empty())
			{
				
				auto vit = vstemp.top();
				vtemp.push_back(vit->currentIndexPos);
				
				vstemp.pop();
			}

			sort(vtemp.begin(), vtemp.end());
			if(true == AddUniqueSolutionOnly(vtemp))
				vSolutions.push_back(vtemp);
		}
		
		//cout << "\n\nNO more space to place the next queen vs=["<<vs.size()<<"], pop and rearrange **";
		currPos = RearrangeLastQueen(v, vs);

	}
	
	PrintSolutions(vSolutions);

	return;
}

void NewQueenSolution::PrintSolutions(VCOLLECTION &vSln)
{
	std::lock_guard<std::mutex> lock(g_i_mutexRead);
	int counter = 0;
		for (auto vitr = vSln.begin(); vitr != vSln.end(); ++vitr)
		{
			cout << "\nSol - " << ++counter << " : ";

			for (auto& val : *vitr)
			{
				cout << "[" << val << "], ";
			}

		}

	return;
}

void NewQueenSolution::PrintStackInfo(stack <std::shared_ptr<QueenPlacedAt>>vstemp)
{
	while (!vstemp.empty())
	{
		cout << "\nQid = " << vstemp.top()->queenId
			<< ", TotalIndexes = " << vstemp.top()->availableIndexes.size()
			<< ", Queen Moved Times = " << vstemp.top()->_queenMovedCount
			<< ", answer Position = " << vstemp.top()->currentIndexPos;
		
		vstemp.pop();
	}
	return;
}

void NewQueenSolution::mark_unavailable_indexes(VINT &v, int currentIndexPos)
{
	if (currentIndexPos < 0 || currentIndexPos >(x_length*y_length) - 1)
		return;

	mark_horizontal(v, currentIndexPos);
	mark_veritcal(v, currentIndexPos);
	mark_diagonals(v, currentIndexPos);
}

int NewQueenSolution::convert_2d_to_index(int xpos, int ypos)
{
	return ypos*x_length + xpos;
	
}

void NewQueenSolution::mark_horizontal(VINT& v, int currentIndexPos)
{
	int yCol = currentIndexPos / y_length;
	int index = yCol * x_length;

	for(int x = index; x < index + x_length; ++x )
	{
		v[x] = 1;
	}
}

void NewQueenSolution::mark_veritcal(VINT& v, int currentIndexPos)
{
	int index = currentIndexPos % x_length;

	for (int x = 0; x < x_length; ++x)
	{
		v[index]  = 1;
		index += x_length;
	}

}

void NewQueenSolution::mark_diagonals(VINT& v, int index)
{
	int x_currpos = index % x_length;
	int y_currpos = index / y_length;
	
	int hptr = x_currpos;
	int vptr = y_currpos;

	int x0 = 0, ymax = y_length - 1;
	//move left + top
	while (hptr > x0 && vptr < y_length -1)
	{
		v[convert_2d_to_index(--hptr, ++vptr)] = 1;
	}

	hptr = x_currpos;
	vptr = y_currpos;
	//move right + bottom
	while (hptr < x_length-1 && vptr > 0)
	{
		v[convert_2d_to_index(++hptr, --vptr)] = 1;
	}

	hptr = x_currpos;
	vptr = y_currpos;
	//move right + top
	while (hptr < x_length-1 && vptr < y_length-1)
	{
		v[convert_2d_to_index(++hptr, ++vptr)] = 1;
	}

	hptr = x_currpos;
	vptr = y_currpos;
	//move left + bottom
	while (hptr > 0 && vptr > 0 )
	{
		v[convert_2d_to_index(--hptr, --vptr)] = 1;
	}


}



