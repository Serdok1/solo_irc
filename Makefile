FILES = *.cpp
COMPILER = c++
FLAGS = -std=c++98 -Wall -Wextra -Werror
OUTPUT_NAME = IRC_SERVER

all:
	$(COMPILER) $(FLAGS) $(FILES) -o $(OUTPUT_NAME)
clean:
	rm -fr $(OUTPUT_NAME)
re: clean all

PHONY: all clean re
