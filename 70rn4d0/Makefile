CXX=c++
CXXFLAGS=-Wall -Wextra -Werror -std=c++98
OBJ= miniserv.o utils.o
NAME=serv

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