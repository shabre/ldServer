ldServer_exe : main.o tcpServer.o analyzeBuf.o map.o balancer.o benchmark.o
	g++ -lpthread -o ldServer_exe main.o tcpServer.o analyzeBuf.o map.o balancer.o benchmark.o

main.o : main.cpp
	g++ -c main.cpp -o main.o 

tcpServer.o : tcpServer.cpp
	g++ -lpthread -c tcpServer.cpp -o tcpServer.o 

analyzeBuf.o : analyzeBuf.cpp
	g++ -c analyzeBuf.cpp -o analyzeBuf.o

map.o : map.cpp
	g++ -c map.cpp -o map.o

balancer.o : balancer.cpp
	g++ -c balancer.cpp -o balancer.o

benchmark.o : benchmark.cpp
	g++ -c benchmark.cpp -o benchmark.o

clean: 
	rm *.o
