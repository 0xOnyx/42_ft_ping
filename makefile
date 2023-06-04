NAME = ft_ping

COLOR_ESC			= \033
COLOR_WHITE			= $(COLOR_ESC)[97m
COLOR_CYAN			= $(COLOR_ESC)[96m
COLOR_GREEN			= $(COLOR_ESC)[32m
COLOR_BLUE			= $(COLOR_ESC)[34m
COLOR_YELLOW		= $(COLOR_ESC)[33m
COLOR_MAGENTA 		= $(COLOR_ESC)[35m
COLOR_BOLD			= $(COLOR_ESC)[1m
COLOR_RESET			= $(COLOR_ESC)[0m
COLOR_RESET_BOLD	= $(COLOR_ESC)[21m

PATH_HEADER			= inc/
PATH_SRC			= src/
PATH_OBJ			= objs/

HEADER				= ft_ping.h
SRC					= ft_ping.c proc_v4.c proc_v6.c readloop.c send_v4.c send_v6.c utils.c

SRCS				= $(addprefix $(PATH_SRC),$(SRC))

OBJ					= $(SRCS:.c=.o)
OBJS				= $(addprefix $(PATH_OBJ),$(OBJ))
HEADERS				= $(addprefix $(PATH_HEADER),$(HEADER))

DEBUGING			= -g3 #-fsanitize=address -fsanitize=undefined
CFLAGS				= -Wall -Wextra -Werror -Wpedantic

OPTIONS				= -I$(PATH_HEADER)
LIB					= -lm
CC					= gcc
RM					= rm -rf

DEBUG = 1

ifeq ($(DEBUG), 1)
	CFLAGS += $(DEBUGING)
else
	CFLAGS += -Ofast
endif

all			: $(NAME)

$(PATH_OBJ)$(PATH_SRC)%.o	: $(PATH_SRC)%.c $(HEADERS)
	@mkdir -p $(PATH_OBJ)$(PATH_SRC)
	@$(CC) $(CFLAGS) $(OPTIONS) -o $(@) -c $(<)
	@printf "$(COLOR_GREEN)[$(COLOR_WHITE)INFO$(COLOR_GREEN)] COMPILATION $(COLOR_BLUE)DEBUG => [%s] $(COLOR_BOLD)SRC\t\t=>\t$(COLOR_WHITE)%s$(COLOR_RESET)\n" $(DEBUG) $<


$(NAME)		: $(OBJS)
	@$(CC) $(CFLAGS) $(OPTIONS) -o $(@) $(OBJS) $(LIB)
	@echo "$(COLOR_GREEN)[$(COLOR_WHITE)INFO$(COLOR_GREEN)] LINKAGE $(COLOR_BOLD)ALL OBJS FILE =>\n\t $(COLOR_WHITE)$(OBJS:.o=.o\n\t)"
	@echo "$(COLOR_GREEN)[$(COLOR_WHITE)INFO$(COLOR_GREEN)] COMPILATION FINISH !$(COLOR_WHITE)$(COLOR_RESET_BOLD)"

clean		:
	@$(RM) $(OBJS)
	@$(RM) $(PATH_OBJ)
	@echo "$(COLOR_GREEN)[$(COLOR_WHITE)INFO$(COLOR_GREEN)] DELETE $(COLOR_BOLD)ALL OBJS FILE $(NAME) =>\n\t $(COLOR_WHITE)$(OBJS:.o=.o\n\t)"
	@echo "$(COLOR_GREEN)[$(COLOR_WHITE)INFO$(COLOR_GREEN)] CLEAN FINISH !$(COLOR_RESET)"

fclean		: clean
	@$(RM) $(NAME)
	@echo "$(COLOR_GREEN)[$(COLOR_WHITE)INFO$(COLOR_GREEN)] FCLEAN FINISH !$(COLOR_RESET)"

re			: fclean all

.PHONY: all fclean clean re