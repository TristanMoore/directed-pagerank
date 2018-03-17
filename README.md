# directed-pagerank
Reads directed graph, then calculates pagerank. Multithreaded


"Usage: -f <graphFileName> -th <Thread Ct>"


This program reads and stores a directed graph from edgelist file. Filename and thread count are from commandline args.
It uses the pagerank algorithm to calulate pagerank values. Based off threadcount, it chooses
between two PageRank algorithms (threaded and unthreaded, each optimized).
After calculation, it quicksorts and prints LIMIT amount of ranks.
