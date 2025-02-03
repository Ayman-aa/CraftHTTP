BOLD := \033[1m
GREEN := \033[0;32m
YELLOW := \033[0;33m
RED := \033[0;31m
BLUE := \033[0;34m
NC := \033[0m


all:
	@c++ -g3  -std=c++98 -Wall -Wextra -Werror main.cpp ./config/*.cpp ./server/*.cpp -I/includes/ -o srv
	@printf "$(GREEN)$(BOLD)Building is done!$(NC)\n"
clean:
	@rm -rf srv
	@printf "$(YELLOW)$(BOLD)Cleaning up...$(NC)\n"
re: clean all
