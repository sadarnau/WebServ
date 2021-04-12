# COLORS
_RED		=	\e[1;31m
_GREEN		=	\e[1;32m
_YELLOW		=	\e[1;33m
_BLUE		=	\e[1;34m
_END		=	\e[1;0m

NAME =			webserv

# SOURCES
SRC =			$(SRC_NAME)
SRC_PATH =		sources/

SRC_NAME =		main.cpp Request.cpp Response.cpp webServ.cpp

# HEADER
HEAD_PATH =		includes/

# All .o (object file)
OBJS_PATH =		objs/
OBJS =			$(SRC:%.cpp=$(OBJS_PATH)%.o)

# Compilation
CLANG =			clang++

# Compilation flags
CLANGFLAGS =	-Werror -Wall -Wextra

all:			$(OBJS)
				@$(CLANG) $(CLANGFLAGS) $(OBJS) -o $(NAME)
				@printf "\033[2K\r$(_GREEN)All done!$(_END)\n"
				
$(NAME):	all

# Getting .o from .c rule
$(OBJS_PATH)%.o: $(SRC_PATH)%.cpp
				@printf "\033[2K\r $(_YELLOW)Compiling $< $(_END)⌛"
				@$(CLANG) $(CLANGFLAGS) -I $(HEAD_PATH) -c $< -o $@

$(OBJS):		$(OBJS_PATH)

$(OBJS_PATH):
				@echo "\033[1;32mCreating webserv :\n\033[m"
				@mkdir $(OBJS_PATH)

fclean:
				@rm -rf $(OBJS_PATH)
				@rm -rf $(NAME)
				@echo "\033[1;31mDeleting our webserv </3\033[m"

clean:
				@rm -rf $(OBJS_PATH)
				@echo "\033[1;31mDeleting all object files </3\033[m"

re:				fclean all

.PHONY:			all clean fclean re