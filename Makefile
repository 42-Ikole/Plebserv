NAME		=	plebserv

CC			=	clang++
FLAGS		=	-std=c++98 -pedantic -Wall -Werror -Wextra 
DEBUG_FLAGS	=	-g -fsanitize=address
SRC			=	src/cgi.cpp \
				src/header.cpp \
				src/location.cpp \
				src/main.cpp \
				src/parser_config.cpp \
				src/plebception.cpp \
				src/run_helper.cpp \
				src/run.cpp \
				src/server_response.cpp \
				src/server.cpp \
				src/utilities.cpp

RM 			=	rm -rf

# OS detection for libs and headers
UNAME_S			:=	$(shell uname -s)

ifeq ($(UNAME_S),Linux)
FLAGS			+=	-DLINUX
endif #Linux

ifeq ($(UNAME_S),Darwin)
endif #Darwin


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

superclean:
	$(RM) $(NAME) html/* tester

release:
	@clear
	@$(CC) $(FLAGS) $(SRC) $(LIBS) -Ofast -I includes -o $(NAME) &
	@echo "   ___ _      _                         "
	@sleep 1
	@echo "  / _ \ | ___| |__  ___  ___ _ ____   __"
	@sleep 0.5
	@echo " / /_)/ |/ _ \ '_ \/ __|/ _ \ '__\ \ / /"
	@sleep 1
	@echo "/ ___/| |  __/ |_) \__ \  __/ |   \ V / "
	@sleep 0.4
	@echo "\/    |_|\___|_.__/|___/\___|_|    \_/  "
	@/bin/echo -n "___" && sleep 0.1 && /bin/echo -n "___" && sleep 0.3 \
	&& /bin/echo -n "___" && sleep 0.2 && /bin/echo -n "___" && sleep 0.1 \
	&& /bin/echo -n "___" && sleep 0.1 && /bin/echo -n "___" && sleep 0.2 \
	&& /bin/echo -n "____" && sleep 0.3 && /bin/echo -n "___" && sleep 0.1 \
	&& /bin/echo -n "___" && sleep 0.1 && /bin/echo -n "___" && sleep 0.2 \
	&& /bin/echo -n "____" && sleep 0.2 && /bin/echo "_____" && sleep 0.5
	@echo "A shitty webserver written by plebians [no plagiarism, no pet @rpet91]"
	@echo "Made by:"
	@sleep 0.4
	@echo "K1ngmar:	| https://github.com/K1ngmar"
	@sleep 0.4
	@echo "VictorTennekes:	| https://github.com/VictorTennekes"
	@sleep 0.4
	@echo "Alpha1337k:	| https://github.com/Alpha1337k"