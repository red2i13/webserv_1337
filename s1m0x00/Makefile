CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98

FILES = server.cpp HttpRequest.cpp
OBJS = $(FILES:.cpp=.o)
NAME = server

$(NAME): $(OBJS)
		$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
		$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean fclean re

clean:
		rm -f $(OBJS)

fclean: clean
		rm -f $(NAME)

re: clean fclean $(NAME)