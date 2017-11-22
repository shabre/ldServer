ldServer_exe : main.o tcpServer.o analyzeBuf.o 
	g++ -lpthread -o ldServer_exe main.o tcpServer.o analyzeBuf.o

main.o : main.cpp
	g++ -c main.cpp -o main.o 

tcpServer.o : tcpServer.cpp
	g++ -lpthread -c tcpServer.cpp -o tcpServer.o 

analyzeBuf.o : analyzeBuf.cpp
	g++ -c analyzeBuf.cpp -o analyzeBuf.o



clean: 
	rm *.o
