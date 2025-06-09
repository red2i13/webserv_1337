CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g3
CC = c++
NAME = Convert
FILES = main.cpp socket_loop.cpp
OBJECTS = $(FILES:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJECTS)
        $(CC) $(CPPFLAGS) $^ -o $@

%.o: %.cpp
        $(CC) -c $(CPPFLAGS) -o $@ $<

clean:
        rm -rf *.o

fclean: clean
        $(RM) $(NAME)
re: fclean all

.PHONY: clean fclean re