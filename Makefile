NAME		=	plebserv

CC			=	clang++
FLAGS		=	# -std=c++98 # -pedantic -Wall -Werror -Wextra 
DEBUG_FLAGS	=	-g -fsanitize=address
SRC			=	src/*.cpp
# SRC			=	src/main.cpp \
# 				src/error.cpp \
# 				src/parser_config.cpp \
# 				src/server.cpp
LIBS		=	
OBJ			=	
RM 			=	rm -f

all: $(NAME)

$(NAME): $(SRC)
	$(CC) $(FLAGS) $(SRC) $(LIBS) -I includes -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

run: re
	./$(NAME)

debug: fclean
	$(CC) $(FLAGS) $(DEBUG_FLAGS) $(SRC) $(LIBS) -I includes -o $(NAME)
