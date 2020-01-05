all:	pair2net findpath
pair2net:	pair2net.cpp
	g++ pair2net.cpp -o pair2net -Ofast -Wall -static -s
findpath:	findpath.cpp
	g++ findpath.cpp -o findpath -Ofast -Wall -static -s

