NAME = ircserv
FLAGS = c++ -Wall -Wextra -Werror -std=c++98
SRCS = main.cpp Server.cpp Client.cpp Utils.cpp ./commands/join.cpp ./commands/privmsg.cpp ./commands/names.cpp ./commands/whois.cpp ./commands/quit.cpp ./commands/topic.cpp
HPP = Server.hpp Client.hpp Errors.hpp Utils.hpp
OBJECTS = $(SRCS:.cpp=.o)

$(NAME): $(OBJECTS) $(HPP)
	@$(FLAGS) $(SRCS) -o $(NAME)

all: $(NAME) $(OBJECTS)

%.o : %.cpp 
	@$(FLAGS) -c $< -o $@
clean:
	@rm -rf $(OBJECTS)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re