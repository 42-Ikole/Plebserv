NAME=plebserv

CC=clang++
FLAGS=-Wall -Werror -Wextra -std=c++98 -pedantic
DEBUG_FLAGS=-g -fsanitize=address
SRC=src/*.cpp
LIBS=
OBJ=
RM =rm -rf

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $(FLAGS) $(SRC) $(LIBS) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

run: re
	./$(NAME)

debug: fclean
	$(CC) $(FLAGS) $(DEBUG_FLAGS) $(SRC) $(LIBS) -o $(NAME)
