all: 
	c++ -std=c++98 -Wall -Wextra -Werror -o serv main.cpp classes/*.cpp config/*.cpp -I./includes -I./config