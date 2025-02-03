NAME = webserv
CXX = c++
CXXFLAGS = -std=c++98 -Wall -Wextra -Werror
INC = -I./includes

# Source directories
SRC_DIR = .
CLASS_DIR = classes

# Source files
SRC = $(wildcard $(SRC_DIR)/*.cpp)
CLASS_SRC = $(wildcard $(CLASS_DIR)/*.cpp)
ALL_SRC = $(SRC) $(CLASS_SRC)

# Object files
OBJ_DIR = obj
OBJ = $(ALL_SRC:%.cpp=$(OBJ_DIR)/%.o)

# Create object directories
$(OBJ_DIR)/$(CLASS_DIR)/%.o: $(CLASS_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)/$(CLASS_DIR)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# Main rules
all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re