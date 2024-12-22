all: 
	c++ -std=c++98 -Wall -Wextra -Werror main.cpp classes/*.cpp -I./includes -o webserv
