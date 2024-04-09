NAME = ircserv

SRC = main.cpp Server.cpp Client.cpp

OBJ = ${SRC:.cpp=.o}

CXX = c++

CXXFLAGS = -std=c++98 -fsanitize=address -g3 #-Wall -Wextra -Werror 

all : $(NAME)

$(NAME) : $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)

clean :
	rm -rf *.o

fclean : clean
	rm -rf $(NAME)

re : fclean all