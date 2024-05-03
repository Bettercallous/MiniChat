NAME = ircserv

SRC = main.cpp ./Srcs/Server.cpp ./Srcs/Client.cpp ./Srcs/Channel.cpp ./Srcs/Authentication.cpp \
		./Srcs/Broadcast.cpp ./Srcs/Commands.cpp ./Srcs/Helpers.cpp ./Srcs/Modes.cpp

OBJ = ${SRC:.cpp=.o}

CXX = c++

CXXFLAGS = -std=c++98 -fsanitize=address -g3 -Wall -Wextra -Werror -I ./Includes

all : $(NAME)

$(NAME) : $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean :
	rm -rf *.o

fclean : clean
	rm -rf $(NAME)

re : fclean all