all: 
	c++ -std=c++98 -Wall -Wextra -Werror -pthread -g main.cpp classes/*.cpp -I./includes -o webserv
