/*
Tristan Moore Asst09 Section 1002

This program is multithreaded. Default value is 4, but optimal is however many cores your CPU has.

This program reads and stores a directed graph from edgelist file. Filename and thread count are from commandline args.
It uses the pagerank algorithm to calulate pagerank values. It chooses from two pagerank algorithms based off passed threadCt.
After calculation, it quicksorts and prints "amt" values of calculated pageRanks.
*/

#pragma once
#include <string>
#include <mutex>

using namespace std;


class directedGraph
{
private:
	//class structs
	struct node
	{
		node *link;
		int item;
	};
	struct pagerank
	{
		int pagenum;
		double rankval;
	};

	//Assignment 11 Pagerank algo specifications
	const int PR_ITERATIONS = 100;			//how many iterations until pagerank reaches convergence
	const double DAMP_FACTOR = 0.85;			//dampening factor for pageRank algo

												//graphfile declarations
	int vertexCt;
	int edgeCt;
	string title;
	int *countList;			//stores number of outlinks from item at x. "How many things does [x] point to" ?
	node **inlinks;			//stores inlinks to x. "What points to [x]" ?
	pagerank *pageRanks;	//stores calculated pageranks

	//threading declarations for findPageRankTHREADED()
	const int MAX_THREADS = 32;	//this is max threads allowed
	int THREAD_AMT = 4;			//this is the default threads
	int p_curr;		//mutexed in subPR
	int iter_curr;			//current iterations for PR_ITERATIONS
	int bool_index;	//gives threads a global index for thread_joiner. mutexed in subPR
	bool *thread_joiner;	//works like thread.join() , but without deleting the threads
	double *newPR_T;
	double damp_with_spread_T;
	mutex mtx;
	mutex mtxb;

	//private functions
	void findPageRanksTHREADED();
	void findPageRanksUNTHREADED();
	void subPR();
	void quicksort(int, int);
	void insertionSort(int, int);
	void printGraph();				//used for debugging


public:
	//public functions
	directedGraph();
	~directedGraph();
	bool readGraph(string);
	void setThreadCt(int);
	void showGraphStats();
	void findPageRanks();
	void displayPageRanks(int);

};
