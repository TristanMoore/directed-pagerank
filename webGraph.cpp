/*
Tristan Moore Asst09 Section 1002

Program information is in header webGraph.h
*/

#include "webGraph.h"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <thread>
#include <mutex>

#include <chrono>
#include <atomic>

using namespace std;



//---------PUBLIC FUNCTIONS---------

//constructor. initializes essential values and pointers to NULL or 0 values
directedGraph::directedGraph()
{
	vertexCt = edgeCt = 0;
	title = "";
	countList = nullptr;
	inlinks = nullptr;
	pageRanks = nullptr;
}//done

 //destructor. deallocates dynamic arrays and linked adjacency lists from inlinks[]
directedGraph::~directedGraph()
{
	node *tmpnext, *prev;

	delete[] countList;
	delete[] pageRanks;

	if (inlinks != nullptr)					//deletes all nodes in linkedList array
		for (int i = 0; i < vertexCt; i++)
		{
			tmpnext = inlinks[i];
			while (tmpnext != nullptr)
			{
				prev = tmpnext;
				tmpnext = tmpnext->link;
				delete prev;
			}
		}
}//done

 //reads formatted graph file from passed filename. returns false if unable to open, true after reading and storing file.
//ignores edges that contain a vertex out of ranges
bool directedGraph::readGraph(string filename)
{
	string tmp;
	char hashchar;		//not for a hash, just gets rid of '#'
	int fromint, toint;
	node *insertnode;
	ifstream graph;

	//open file
	graph.open(filename);
	if (!graph.is_open())
		return false;

	//read header of graph file
	getline(graph, tmp);			//first line, dont need

	graph >> hashchar;				//get rid of #
	graph.get(hashchar);
	getline(graph, tmp);			//holds title
	title = tmp;

	graph >> hashchar;				//get rid of #
	graph >> tmp >> tmp;			//first "Nodes:" then number
	vertexCt = stoi(tmp);

	graph >> hashchar;				//get rid of #
	graph >> tmp >> tmp;			//first "Edges:" then number
	graph >> hashchar;
	edgeCt = stoi(tmp);


	getline(graph, tmp);			//dont need "node-to-node" line

									//allocate memory for countList,inlinks,pageRanks and initilizes both to 0,nullptr,0.0
	countList = new int[vertexCt + 1]();
	inlinks = new node*[vertexCt]();
	pageRanks = new pagerank[vertexCt]();
	for (int i = 0; i < vertexCt; i++)
		pageRanks[i].pagenum = i;

	//read and store data from graph file
	for (int i = 0; i < edgeCt; i++)
	{
		graph >> fromint >> toint;

		//code below inserts to front of inlinks[i]
		if (fromint < vertexCt && toint < vertexCt)		//error checks to make sure passed edge src and dest are in range
		{
			insertnode = new node;
			insertnode->item = fromint;
			insertnode->link = inlinks[toint];
			inlinks[toint] = insertnode;

			//increments appropriate countList item
			countList[fromint]++;
		}
	}

	return true;

}//done

 //sets thread count
void directedGraph::setThreadCt(int count)
{
	if (count > MAX_THREADS)
	{
		cout << "Warning: max Threads allowed is " << MAX_THREADS;
		cout << ". Running with default: " << THREAD_AMT << " threads." << endl << endl;
	}
	else if (count <= 1)
	{
		cout << "Specified threads is less than or equal to 1." << endl;
		cout << "Running the (probably) SLOWER unthreaded PageRank algorithm instead." << endl << endl;
		THREAD_AMT = 0;
	}
	else
	{
		cout << "Thread count is set to " << count << "." << endl << endl;
		THREAD_AMT = count;
	}

}

//prints graph statistics after reading file
void directedGraph::showGraphStats()
{
	cout << "Graph Statistics:" << endl;
	cout << "   Title: " << title << endl;
	cout << "   Nodes: " << vertexCt << endl;
	cout << "   Edges: " << edgeCt << endl;

	cout << endl;
}//done

 //Should be called after setThreadCt. Determines which algo to use. (each is optimized for with/without threads).
void directedGraph::findPageRanks()
{
	if (THREAD_AMT == 0)
		findPageRanksUNTHREADED();

	else
		findPageRanksTHREADED();
}

//Should be called after findPageRanks. displays formatted, ordered pageRanks[]
void directedGraph::displayPageRanks(int amt)
{
	//sort pageRanks[] based on value
	quicksort(0, vertexCt - 1);
	
	//subsort pageRanks[] based on pageNum via insertionsort
	int lo = 0, hi = 1;
	while (hi < vertexCt)
	{
		if (pageRanks[lo].rankval == pageRanks[hi].rankval)	//if a subarray is found
		{
			while (pageRanks[lo].rankval == pageRanks[hi].rankval)//find range of subarray
			{
				hi++;
			}

			insertionSort(lo, hi);	//sort subarray

			lo = hi - 1;	//set lo and high just after subarray so main while loop can continue
		}

		lo++;
		hi++;
	}



	//display header
	cout << "Page Ranks:" << endl << "-----------" << endl;
	cout << right << setprecision(8) << fixed;
	cout << setw(5) << "Rank";
	cout << setw(10) << "Page";
	cout << setw(17) << "Rank Value";
	cout << endl;

	//display formatted, sorted values
	for (int i = 0; i < amt && i < vertexCt; i++)
	{
		cout << setw(5) << i + 1;
		cout << setw(10) << pageRanks[i].pagenum;
		cout << setw(17) << pageRanks[i].rankval;
		cout << endl;
	}
	cout << endl;
}//done



 //---------PRIVATE FUNCTIONS---------

 //threaded pagerank algorithm
void directedGraph::findPageRanksTHREADED()
{
	//TIMER START
	//		auto tm1 = chrono::high_resolution_clock::now();

	//various algorithm variables
	double N = double(vertexCt);
	double sinkPR;
	newPR_T = new double[vertexCt];
	int *S = new int[vertexCt]();
	int s_size = 0;

	//precalculated for algo below
	double damping = (1.0 - DAMP_FACTOR) / N;
	double one_div_N = 1.0 / N;

	//for Threading
	thread *threadArr = new thread[THREAD_AMT];
	iter_curr = 0;
	bool_index = 0;		//
	bool threads_created = false;
	thread_joiner = new bool[THREAD_AMT];
	bool join_sum = false;
	//all of the above vars allow me to run the same threads for each of the 100 PR iterations, as
	//oppsed to creating and joining a new set of threads for each iteration.


	//--------------------ALGO START------------------------

	// S is the set of sink nodes (pages with no outlinks)
	for (int i = 0; i < vertexCt; i++)
	{
		if (countList[i] == 0)
		{
			S[s_size] = i;
			s_size++;
		}
	}

	//for each page p in P
	for (int i = 0; i < vertexCt; i++)
	{
		pageRanks[i].rankval = one_div_N;		//set initial value
	}


	//while PageRank has not converged do
	while (iter_curr < PR_ITERATIONS)
	{
		sinkPR = 0;

		//for each page p in S
		for (int i = 0; i < s_size; i++)
		{
			sinkPR += pageRanks[S[i]].rankval;
		}

		damp_with_spread_T = damping + (DAMP_FACTOR)*(sinkPR) / N;	//precalculated for for loop below

																	//for each page p in P THREADED
		for (int i = 0; i < THREAD_AMT; i++)
		{
			thread_joiner[i] = false;		//release the Kracke-- ...threads! 
		}

		p_curr = 0;
		if (!threads_created)
		{
			threads_created = true;
			for (int i = 0; i < THREAD_AMT; i++)
				threadArr[i] = thread(&directedGraph::subPR, this);	//start the threads
		}



		//wait in a loop until all threads are done
		join_sum = false;
		while (!join_sum)
		{
			join_sum = true;
			for (int i = 0; i < THREAD_AMT; i++)
			{
				if (!thread_joiner[i])
					join_sum = false;
			}
		}

		//for each page p 
		for (int p = 0; p < vertexCt; p++)
		{
			pageRanks[p].rankval = newPR_T[p];	//copy newPR to main pageRanks
		}


		iter_curr++;
	}//end of "while not converged"


	 //frees, destroys, and terminates leftover threads
	for (int i = 0; i < THREAD_AMT; i++)
		thread_joiner[i] = false;
	for (int i = 0; i < THREAD_AMT; i++)
		threadArr[i].join();

	//deallcate dynamic arrays
	delete[] newPR_T;
	delete[] S;

	//TIMER END
	//auto tm2 = chrono::high_resolution_clock::now();
	//cout << endl;
	//cout << "Program took: " << std::chrono::duration_cast<std::chrono::milliseconds>
	//	(tm2 - tm1).count() << " milliseconds" << endl << endl;

}

//unthreaded pagerank algorithm. This exists to avoid thread creation,mutex,etc overhead when running a single thread.
//.... but why would you use this one, when you can use the faster THREADED?
void directedGraph::findPageRanksUNTHREADED()
{
	//TIMER START
	//	auto tm1 = chrono::high_resolution_clock::now();

	//various algorithm variables
	double N = double(vertexCt);
	double sinkPR;
	double *newPR = new double[vertexCt];
	int *S = new int[vertexCt]();
	int s_size = 0;

	//precalculated for algo below
	double damping = (1.0 - DAMP_FACTOR) / N;
	double one_div_N = 1.0 / N;

	//--------------------ALGO START------------------------

	// S is the set of sink nodes (pages with no outlinks)
	for (int i = 0; i < vertexCt; i++)
	{
		if (countList[i] == 0)
		{
			S[s_size] = i;
			s_size++;
		}
	}

	//for each page p in P
	for (int i = 0; i < vertexCt; i++)
	{
		pageRanks[i].rankval = one_div_N;		//set initial value
	}


	//while PageRank has not converged do
	for (int c = 0; c < PR_ITERATIONS; c++)
	{
		sinkPR = 0;

		//for each page p in S
		for (int i = 0; i < s_size; i++)
		{
			sinkPR += pageRanks[S[i]].rankval;
		}

		//for each page p in P
		double damp_with_spread = damping + (DAMP_FACTOR)*(sinkPR) / N;	//precalculated for for loop below
		for (int p = 0; p < vertexCt; p++)
		{
			newPR[p] = damp_with_spread;		//combined "damping" and "spread of sink PR evenly"

												//for each page q in M(p)
			node *iter = inlinks[p];
			while (iter != nullptr)
			{
				int q = iter->item;
				newPR[p] += (DAMP_FACTOR)*(pageRanks[q].rankval) / countList[q];	//add share of PageRank from inlinks
				iter = iter->link;
			}
		}


		//for each page p. 
		for (int p = 0; p < vertexCt; p++)
		{
			pageRanks[p].rankval = newPR[p];	//copy newPR to main pageRanks
		}
	}//end of "while not converged" (thats actually a for loop)


	 //deallcate dynamic arrays
	delete[] newPR;
	delete[] S;

	//TIMER END
	//	auto tm2 = chrono::high_resolution_clock::now();
	//	cout << endl;
	//	cout << "Program took: " << std::chrono::duration_cast<std::chrono::milliseconds>
	//		(tm2 - tm1).count() << " milliseconds" << endl << endl;

}//done

 //subroutine called in threaded findPageRank(). Always and only ran via worker threads. Part of pageRank algorithm.
void directedGraph::subPR()
{
	int p;
	mtxb.lock();
	int b_i = bool_index++;	//b_i is thread specific bool index for bool_joiner. bool_index is atomic.
	mtxb.unlock();

	while (iter_curr < PR_ITERATIONS)	//fPR will inc iter_curr
	{
		mtx.lock();
		p = p_curr++;	//p_curr++ is mutexd, to prevent race conditions
		mtx.unlock();

		while (p < vertexCt)
		{
			newPR_T[p] = damp_with_spread_T;		//combined "damping" and "spread of sink PR evenly"

													//for each page q in M(p)
			node *iter = inlinks[p];
			while (iter != nullptr)
			{
				int q = iter->item;
				newPR_T[p] += (DAMP_FACTOR)*(pageRanks[q].rankval) / countList[q];	//add share of PageRank from inlinks
				iter = iter->link;
			}
			mtx.lock();
			p = p_curr++;
			mtx.unlock();
		}

		thread_joiner[b_i] = true;	//let findPageRank know this thread is done.
		while (thread_joiner[b_i])
		{
			//thread waits here. findPageRank will let this thread pass when all threads are done.
		}
	}

}

//standard quicksort, called in displayPageRanks() for pageRanks[]. sorts based off pageval
void directedGraph::quicksort(int lo, int hi)
{
	double pivot;
	int left = lo, right = hi;
	pagerank tmp;

	//choose pivot
	pivot = pageRanks[(lo + hi) / 2].rankval;

	//partition based off pivot
	while (left <= right)
	{
		while (pageRanks[left].rankval > pivot)
			left++;
		while (pageRanks[right].rankval < pivot)
			right--;

		if (left <= right)
		{
			//swap values
			tmp = pageRanks[left];
			pageRanks[left] = pageRanks[right];
			pageRanks[right] = tmp;
			left++;
			right--;
		}
	};

	//recursive quicksort on sub array
	if (lo < right)
		quicksort(lo, right);
	if (left < hi)
		quicksort(left, hi);

}


//used to sort subarrays in Pageranks that have equal rankvales. sorts based off pagenum
void directedGraph::insertionSort(int lo, int hi)
{
	pagerank key;
	int i, j;
	for (j = lo + 1; j < hi; j++)
	{
		key = pageRanks[j];
		for (i = j - 1; (i >= lo) && (pageRanks[i].pagenum > key.pagenum); i--)
			pageRanks[i + 1] = pageRanks[i];					//moves small value up

		pageRanks[i + 1] = key;	//puts key into place
	}
}

//This was used for debugging. prints current stored graph and countList
void directedGraph::printGraph()
{
	node *tmppoint;

	//print inlinks
	cout << "To   |From " << endl;
	for (int i = 0; i < vertexCt; i++)
	{
		cout << i << "   |";
		tmppoint = inlinks[i];
		while (tmppoint != nullptr)
		{
			cout << tmppoint->item << " ";
			tmppoint = tmppoint->link;
		}
		cout << endl;
	}
	cout << endl;

	//print countList, which holds count of outlinks at [x]
	for (int i = 0; i < vertexCt; i++)
		cout << i << " count is: " << countList[i] << endl;

	cout << endl;
}//done


