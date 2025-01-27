all: 
	c++  -Wall -Wextra -Werror -pthread -fsanitize=address -g main.cpp classes/*.cpp -I./includes -o webserv
