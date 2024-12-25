all: 
	c++ -std=c++98  -o serv main.cpp classes/*.cpp config/*.cpp -I./includes -I./config
