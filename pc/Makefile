OBJS = pong.c
CC = gcc
LINKER_FLAGS = -lm -lSDL2 -lSDL2_image -lSDL2_mixer -g
OBJ_NAME = pong

all: $(OBJS)
	$(CC) $(OBJS) $(LINKER_FLAGS) -o $(OBJ_NAME)

clean:
	rm $(OBJ_NAME)
