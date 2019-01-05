# BruteForcePerThreadPerCore
A generic (non-templatized) code that can launch each thread on dedicated each available core for any problem space. It uses the shear strength of hardware concurrency and can run on Linux &amp; Windows (Implemented in C++11 &amp; STL)

Unleash power of Brute Force on Multi Processor / Multi Core Servers......

Long forgotten the very best non-brainy and result oriented techinique "Brute Force" algorithm for solving complicated problems. There are many algorithms as alternate but may be complicated and tricky.

Brute Force is a mighty mechanism or way to find the solution of a problem by using the shear strength of the hardware concurrency. Nowadays, processors are very fast, powerful and have multi cores and each core can be hyperthreaded. 

Implemented code is to find a 'N' Queens problem, ran for 10x10 and 16x16 chess boards, but not using breadth-first technique. For example, finding a solution of 8x8 board placing 8 queens, Very first queen can be placed at 64 places on chess board. Second queen can only be places left and so on. Implenting BruteForce has many memory and threaded challenges, but using current C++14 shared_ptr and threads, locks they can be dealt immaculately.

Uploaded example can run on Windows VS2015 and Linux CentOS or Ubuntu machines. Reference code is generalised and can be used for other implementations where each thread has forced allocation on each core respectively. To apply in your code, update the thread callback with your class initialization.

Application can be run with command line parameters e.g. ./8QueenSolutionBruteForce <x_dimension> <y_dimension> <nCores forced to use>
  
Run 1-> ./8QueenSolutionBruteForce will launch 8x8 queen board and on the max available cores
Run 2-> ./8QueenSolutionBruteForce 8 8 will launch 8x8 queen board and on the max available cores < 64
Run 3-> ./8QueenSolutionBruteForce 8 8 8 will launch 8x8 queen board and will use only 8 cores even higher are available.

While at AMD I had access to EPYC server having 128 cores I ran on 64 cores beyond that Segmentation fault which i didnt debug...



