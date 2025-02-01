all: 
	c++ -std=c++98 -Wall -Wextra -Werror -pthread -fsanitize=address -g main.cpp classes/*.cpp -I./includes -o webserv