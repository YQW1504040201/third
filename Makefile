Exe : post.o
	g++ -o Exe post.o -ljsoncpp -lcurl
main.o : post.cpp
	g++ -g -c post.cpp
clean :
	rm post.o Exe
