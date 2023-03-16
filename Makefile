NAME						:=		ircserv

PATH_SRC				:=		src
PATH_INC				:=		inc
PATH_BUILD			:=		build

SRCS						:=		$(shell find $(PATH_SRC) -name *.cpp)
OBJS						:=		$(SRCS:%.cpp=$(PATH_BUILD)/%.o)
DEPS						:=		$(OBJS:.o=.d)
INC_DIRS				:=		$(shell find $(PATH_INC) -type d)

CC							:=		c++

FLAG_WARN				:=		-Wall -Wextra -Werror
FLAG_SHADOW			:=		-Wshadow -Wno-shadow	
FLAG_STD				:=		-std=c++98
FLAG_MAKE				:=		-MMD -MP
FLAG_DEBUG			:= 		-g
FLAG_OPT				:=		-00
FLAG_COMP				:=		$(FLAG_WARN) $(FLAG_STD) $(FLAG_MAKE) $(FLAG_DEBUG) $(FLAG_MLEAK) $(FLAG_SHADOW)

FLAG_MLEAK			:= 		-fsanitize=address

RM							:=		rm -rf

_YELLOW					:=		\e[38;5;184m
_GREEN					:=		\e[38;5;46m
_RESET					:=		\e[0m
_INFO						:=		\$(_YELLOW)INFO$(_RESET)]
_SUCCESS				:=		\$(_GREEN)SUCCESS$(_RESET)]

all:						init $(NAME)
								@ printf "$(_SUCCESS) Compilation done\n"

init:	
								@ printf "$(_INFO) Initialize $(NAME)\n"

$(NAME):				$(OBJS)
								@ $(CC) $(FLAG_COMP) -o $@ $(OBJS)

$(PATH_BUILD)/%.o:	%.cpp
								@ mkdir -p $(dir $@)
								@ $(CC) $(FLAG_COMP) -c $< -o $@

clean:					
								@ $(RM) $(PATH_BUILD)
								@ printf "$(_INFO) Deleted files and directory\n"

fclean:					clean
								@ $(RM) $(NAME)

re:							fclean all

.PHONY:					all clean fclean re

-include $(DEPS)