NAME := webserv
CC := c++

CFLAGS ?= -Wall -Werror -Wextra -std=c++98 -Wshadow -Wno-shadow
ifdef DEBUG
CFLAGS := -g $(CFLAGS)
endif

CLASSES := Location Server Socket
OBJECTS := autoindex.o codes.o config.o constructResponse.o location_block_util.o \
			main.o methods.o parseRequest.o server_block_util.o SocketUtils.o \
			spawnProcess.o utilsMethods.o 
HEADER_FILES := autoindex.hpp codes.hpp config.hpp constructResponse.hpp \
			methods.hpp parseRequest.hpp spawnProcess.hpp utilsMethods.hpp
INCLUDE := headers

OBJECTS := $(OBJECTS) $(addsuffix .o,$(CLASSES))
OBJECTS := $(addprefix obj/,$(OBJECTS))
INCLUDE := $(addprefix -I,$(INCLUDE))
HEADER_FILES := $(HEADER_FILES) $(addsuffix .hpp,$(CLASSES))
HEADER_FILES := $(addprefix headers/,$(HEADER_FILES))

all:
	@#-j4 makes the compilation use 4 threads for faster compiling
	$(MAKE) $(NAME) -j4

$(NAME): cgi-check $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(NAME)

obj/%.o: src/%.cpp $(HEADER_FILES)
	@mkdir -p obj
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -f $(OBJECTS)
	@rm -rf obj

fclean: clean
	rm -f $(NAME)
	rm -f cgi-check

re: fclean all

bonus: all

run: all
	./$(NAME)

cgi-check:
	sh cgi-requirements.sh

debug:
	$(MAKE) DEBUG=1

.PHONY: all clean fclean re bonus run debug
