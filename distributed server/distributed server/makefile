dServer_exe : main.o analyzeBuf.o connect_loadBalancer.o
	g++ -lpthread -o dServer_exe main.o analyzeBuf.o connect_loadBalancer.o

main.o : main.cpp
	g++ -c main.cpp -o main.o 

connect_loadBalancer.o : connect_loadBalancer.cpp
	g++ -c connect_loadBalancer.cpp -o connect_loadBalancer.o 

analyzeBuf.o : analyzeBuf.cpp
	g++ -c analyzeBuf.cpp -o analyzeBuf.o

clean: 
	rm *.o
