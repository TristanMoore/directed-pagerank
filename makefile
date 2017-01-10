OBJS1 = pageRank.o webGraph.o
#OBJS2 = perc.o percGame.o disjointSet.o 

CC = g++
DEBUG = -Wall -Wpedantic -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall -pedantic $(DEBUG)

all: pageRank


#disjointSet.o: disjointSet.cpp disjointSet.h
#	$(CC) $(CFLAGS) disjointSet.cpp
#
#percGame.o: percGame.cpp percGame.h disjointSet.h
#	$(CC) $(CFLAGS) percGame.cpp

#testDS.o: testDS.cpp disjointSet.h
#	$(CC) $(CFLAGS) testDS.cpp

#perc.o: perc.cpp percGame.h disjointSet.h
#	$(CC) $(CFLAGS) perc.cpp

#testDS: $(OBJS1)
#	$(CC) $(LFLAGS) $(OBJS1) -o testDS

#perc: $(OBJS2)
#	$(CC) $(LFLAGS) $(OBJS2) -o perc





#webGraph.o: webGraph.h webGraph.cpp
#	$(CC) $(CFLAGS) webGraph.cpp


#pageRank.o: pageRank.cpp webGraph.h
#	$(CC) $(CFLAGS) pageRank.cpp

#pageRank: $(OBJS1)
#	$(CC) $(LFLAGS) $(OBJS1) -o pageRank



webGraph.o: webGraph.h webGraph.cpp
	$(CC) $(DEBUG) -c webGraph.cpp -pthread -std=c++11


pageRank.o: pageRank.cpp webGraph.h
	$(CC) $(DEBUG) -c pageRank.cpp -pthread -std=c++11

pageRank: $(OBJS1)
	$(CC) $(DEBUG) $(OBJS1) -o pageRank -pthread -std=c++11







clean:
	rm $(OBJS1)
