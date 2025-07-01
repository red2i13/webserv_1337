CXX=c++
CXXFLAGS=-Wall -Wextra -Werror -std=c++98
OBJ= run_cgi.o
NAME=cgi

all:$(NAME)

$(NAME):$(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

run: all
	./$(NAME)

re: fclean all

.SECONDARY: