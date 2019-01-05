#include "NewQueenSolution.h"

int g_x = 0;
int g_y = 0;
int g_ForceCores = 0;
unsigned int TOTAL_PROBLEM_SPACE = 64;

typedef std::shared_ptr<NewQueenSolution> shPtrQSln;
typedef std::vector< shPtrQSln > vshPtrQSlns;

vshPtrQSlns vQSln(TOTAL_PROBLEM_SPACE, 0);

unsigned gTotalCores = std::thread::hardware_concurrency();

//Thread Callback function.
void thread_function(int thCounter);


/*
Following Entry point depicts unique feature implementation of the Brute Force Technique using the high computing power of the
Servers / Machines with greater number of cores/processors[ multi processors have multiple cores ].

>>>>>>
Each thread is assigned to an available / new core. Full cache load and cpu usage can be witnessed while this thread is actively 
associated with the core. 
All threads join once work is finished for synchronisity. A newer batch is launched to repeat the job but from new start position.

In a chess board there are 64 positions available on a fresh board... 
<<<<<<

>>>>>>>>>>>>
CODE IS WRITTEN IN STL AND CAN BE RUN ON WINDOWS VS2015 AND LINUX GLIBC 3.6 OR HIGHER
<<<<<<<<<<<<
*/
int main(int argc, const char** argv) {

	if (argc == 4) {
		g_x = (int)std::stoll(argv[1]);
		g_y = (int)std::stoll(argv[2]);
		gTotalCores = gTotalCores <= std::stoll(argv[3]) ? gTotalCores : (int)std::stoll(argv[3]);

		cout << "\nForced Threds to Cores # = " << gTotalCores;
		TOTAL_PROBLEM_SPACE = g_x * g_y;
		
	}
	else if (argc == 3 )
	{
		g_x = (int)std::stoll(argv[1]);
		g_y = (int)std::stoll(argv[2]);
		TOTAL_PROBLEM_SPACE = g_x * g_y;

	} 
	else{
		cout << "default board 8x8";
		g_x = 8;
		g_y = 8;
		TOTAL_PROBLEM_SPACE = g_x * g_y;
	}

	if (gTotalCores > TOTAL_PROBLEM_SPACE)
		gTotalCores = TOTAL_PROBLEM_SPACE; // Max cores cut down to solution space only needed

	std::cout << "\n\n Program Launched with Problem Space = " << TOTAL_PROBLEM_SPACE
		<< "\n Number of System Cores/Concurrent Threads = " << gTotalCores << endl
		<< " Params X = " << g_x << endl
		<< " Params Y = " << g_y << endl;

	int thCounter = 0;
	int remainder = 0;

	
	int nBatches = TOTAL_PROBLEM_SPACE / gTotalCores < 1 ?
		TOTAL_PROBLEM_SPACE :
		TOTAL_PROBLEM_SPACE / gTotalCores;

	if (TOTAL_PROBLEM_SPACE % gTotalCores > 0 && TOTAL_PROBLEM_SPACE / gTotalCores > 1) // more then 1 batch and remainder 10 /8 ; 1 batch plus + 2 
	{
		remainder = TOTAL_PROBLEM_SPACE % gTotalCores;
	}

	int tempCoresToRun = gTotalCores;
	for (int k = 0; k <= nBatches; ++k)
	{
		if (k == nBatches )
		{ // all batches ran.. now do leftover
			tempCoresToRun = remainder;
		}
		std::vector<std::thread> threads(tempCoresToRun);

		for (int i = 0; i < tempCoresToRun; ++i)
		{

			threads[i] = std::thread(&thread_function, thCounter);
			

#ifdef _NON_WINDOWS			
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			CPU_SET(i, &cpuset);

			int rc = pthread_setaffinity_np(threads[i].native_handle(),
				sizeof(cpu_set_t), &cpuset);

			if (rc != 0) {
				std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
			}
#elif _WINDOWS

			SetThreadAffinityMask(GetCurrentThread(), 1 << i);
#endif
			
			
			++thCounter;

		}
		

		for (auto& t : threads) {
			t.join();
		}


		threads.clear();

		
	}


	cout << "\n\nTotal Solutions Found for [" << g_x << ", " << g_y << "] = " << NewQueenSolution::vSolutions.size() << endl;
	return 0;
}

std::mutex g_i_mutexLaunch;

void thread_function(int thCounter)
{
	shPtrQSln shptr(new NewQueenSolution(gTotalCores, g_x, g_y));
	vQSln[thCounter] = shptr;

	{
		std::lock_guard<std::mutex> lock(g_i_mutexLaunch);

		cout << "\n Starting New Thread Fresh Board as first position = [" << thCounter << "]";
	}
	vQSln[thCounter]->StartFromFreshBoard_Thread(thCounter);

	return;
}
