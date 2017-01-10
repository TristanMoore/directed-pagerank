// CS 302
// Final Project, Google Page Rank Algorithm
/*
Tristan Moore Asst09 Section 1002

This program is multithreaded. Recommended thread count is one less than however many cores your CPU has.
Do to the way this program runs, running more threads than CPU cores will probably be a performace hit.

"Usage: -f <graphFileName> -th <Thread Ct>"

This program reads and stores a directed graph from edgelist file. Filename and thread count are from commandline args.
It uses the pagerank algorithm to calulate pagerank values. Based off threadcount, it chooses
between two PageRank algorithms (threaded and unthreaded, each optimized).
After calculation, it quicksorts and prints LIMIT amount of ranks.
*/


#include <iostream>
#include <string>
#include <cstdlib>
#include "webGraph.h"

using namespace std;

const static int LIMIT = 30;	//number of pagranks to be printed in displayPageRanks()

bool checkArgs(int, char*[]);


// *****************************************************************

int main(int argc, char *argv[])
{

	// ------------------------------------------------------------------
	//  Headers.

	string	stars, bars, dashes;
	string	fName;
	stars.append(65, '*');
	bars.append(65, '=');
	dashes.append(65, '-');
	const char* bold = "\033[1m";
	const char* unbold = "\033[0m";

	cout << stars << endl << bold << "CS 302 - Assignment #11" << endl;
	cout << "Google Page Rank Program" << unbold << endl;
	cout << endl;

	// ------------------------------------------------------------------
	//  Check argument
	//	requires formatted graph file.

	if (!checkArgs(argc, argv))
		return 0;

	// ------------------------------------------------------------------
	//  Read graph and perform page rank operations.

	string	graphFile;
	directedGraph	myGraph;

	myGraph.setThreadCt(stoi(argv[4]));

	graphFile = string(argv[2]);
	if (myGraph.readGraph(graphFile))
	{
		myGraph.showGraphStats();
		myGraph.findPageRanks();
		myGraph.displayPageRanks(LIMIT);	//change passed value to display more pageRanks
	}
	else
	{
		cout << "main: Error reading " << graphFile << "." << endl;
	}


	// ------------------------------------------------------------------
	//  All done.

	cout << stars << endl;
	cout << "Game over, thanks for playing." << endl;

	return 0;
}

//returns true for properly formatted commandline args, else returns false and a message
bool checkArgs(int argc, char *argv[])
{
	if (argc == 1)
	{
		cout << "Usage: -f <graphFileName> -th <Thread Ct>" << endl;
		cout << "Recommended thread count is equal to # of cores in your CPU" << endl << endl;
		return false;
	}
	if (argc != 5)
	{
		cout << "Error: invalid command line options." << endl;
		cout << "Usage: -f <graphFileName> -th <Thread Ct>" << endl;
		cout << "Recommended thread count is equal to # of cores in your CPU" << endl << endl;
		return false;
	}
	if (string(argv[1]) != "-f")
	{
		cout << "Error: invalid file specifier." << endl;
		cout << "Usage: -f <graphFileName> -th <Thread Ct>" << endl;
		cout << "Recommended thread count is equal to # of cores in your CPU" << endl << endl;
		return false;
	}
	if (string(argv[3]) != "-th")
	{
		cout << "Error: invalid thread count specifier." << endl;
		cout << "Usage: -f <graphFileName> -th <Thread Ct>" << endl;
		cout << "Recommended thread count is equal to # of cores in your CPU" << endl << endl;
		return false;
	}

	return true;
}
