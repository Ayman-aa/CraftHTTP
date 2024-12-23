all:
	c++ -g3  -std=c++98 -Wall -Wextra -Werror main.cpp \
		 ./config/*.cpp \
		 -I/includes/     \
		 -o srv
clean:
	rm -rf srv
re: clean all
