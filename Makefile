NAME := webserv
CC := c++

CFLAGS ?= -Wall -Werror -Wextra -std=c++98 -Wshadow -Wno-shadow
ifdef DEBUG
CFLAGS := -g $(CFLAGS)
endif

CLASSES := Example
OBJECTS := main.o
HEADER_FILES :=
INCLUDE := headers

OBJECTS := $(OBJECTS) $(addsuffix .o,$(CLASSES))
OBJECTS := $(addprefix obj/,$(OBJECTS))
INCLUDE := $(addprefix -I,$(INCLUDE))
HEADER_FILES := $(HEADER_FILES) $(addsuffix .hpp,$(CLASSES))
HEADER_FILES := $(addprefix headers/,$(HEADER_FILES))

all:
	@#-j4 makes the compilation use 4 threads for faster compiling
	$(MAKE) $(NAME) -j4

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME)

obj/%.o: src/%.cpp $(HEADER_FILES)
	@mkdir -p obj
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	@rm -rf obj

fclean: clean
	rm -f $(NAME)

re: fclean all

bonus: all

run: all
	./$(NAME)

debug:
	$(MAKE) DEBUG=1

.PHONY: all clean fclean re bonus run debug
