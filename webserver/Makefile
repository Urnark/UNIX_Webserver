# Useful tutorial on Makefiles:
#	http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
########

# To compile the code normaly: 						make
# To compile it in debug mode: 						make DEBUG=1
# To remove the executable and all the .o files: 	make clean

CC = gcc
LD = gcc

INC_DIR = include
SRC_DIR = src
OBJ_DIR = objects

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	de = -ggdb -fsanitize=address -fno-omit-frame-pointer
else
	de = 
endif

# compile time flag to tell gcc where to find header files
CFLAGS = -I$(INC_DIR)

# get all source files in the source directory
SRCS = $(wildcard $(SRC_DIR)/*.c)
# substitute each source file with its corresponding object file
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
PROG = server
RM = /bin/rm

#all: greetings

all: $(PROG)

# greetings: main.o greeter.o
# 	gcc -o greetings main.o greeter.o

$(PROG): $(OBJS)
	$(LD) $^ -o $(PROG) -lpthread $(de)

#main.o: main.c
#	gcc -g -c main.c

#greeter.o: greeter.c
#	gcc -g -c greeter.c

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -lpthread $(de)

#clean:
#	rm main.o greeter.o

clean:
	$(RM) $(PROG) $(OBJS)
