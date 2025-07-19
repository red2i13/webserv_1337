CPPFLAGS = -Wall -Wextra -Werror -std=c++98
CC = c++
NAME = webserv
FILES = main.cpp Http_server.cpp Server_block.cpp HttpRequest.cpp Server_Conf_Parser.cpp HttpResponse.cpp run_cgi.cpp
SOURCES = $(addprefix src/, $(FILES))
OBJECTS = $(SOURCES:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(CPPFLAGS) $^ -o $@

%.o: %.cpp
	$(CC) -c $(CPPFLAGS) -o $@ $<

clean:
	rm -rf src/*.o

fclean: clean
	$(RM) $(NAME)
re: fclean all

.PHONY: clean fclean re