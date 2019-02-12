ABADDON = abaddon
FOLDERS = bin dbg
CC = gcc
SRC = ./src/*.c -I ./src/include/
LIB_FLAGS = -pthread
DEBUG_FLAGS = -g -O0 -o dbg/abaddon_dbg
EXTRA_FLAGS = -Wall -Wextra
FLAGS = -O2 -o bin/abaddon
VALGRING_FLAGS = --tool=memcheck --leak-check=yes --show-reachable=yes \
				 --num-callers=20 --track-fds=yes --track-origins=yes \ 
				 --read-var-info=yes

clear:
	rm -rf $(FOLDERS)

cppcheck:
	mkdir -p dbg
	cppcheck $(SRC) > dbg/lint_log.txt
	
abaddon_compile:
	mkdir -p bin
	$(CC) $(SRC) $(FLAGS) $(LIB_FLAGS)

abaddon:
	bin/abaddon $(PORT) 2> bin/error_log.txt

cgdb_debug:
	mkdir -p dbg
	$(CC) $(SRC) $(DEBUG_FLAGS) $(EXTRA_FLAGS) $(LIB_FLAGS) > dbg/compile_log.txt
	cgdb dbg/abaddon_dbg --args $(PORT)

valgrind_debug:
	mkdir -p dbg
	$(CC) $(SRC) $(DEBUG_FLAGS) $(LIB_FLAGS)
	valgrind $(VALGRIND_FLAGS) dbg/abaddon_dbg $(PORT) 2> dbg/valgrind_log.txt

