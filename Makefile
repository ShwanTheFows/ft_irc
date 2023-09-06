NAME = ircserv
FLAGS = c++ -Wall -Wextra -Werror -std=c++98
SRCS = main.cpp Server.cpp Client.cpp Utils.cpp Channel.cpp ./commands/invite.cpp ./commands/kick.cpp ./commands/list.cpp ./commands/part.cpp ./commands/mode.cpp ./commands/notice.cpp ./commands/join.cpp ./commands/privmsg.cpp ./commands/names.cpp ./commands/whois.cpp ./commands/quit.cpp ./commands/topic.cpp
HPP = Server.hpp Client.hpp Errors.hpp Utils.hpp Channel.hpp
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