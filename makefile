CC=gcc
CFLAGS= -g -O0 -std=c99 -Wall -Werror -Wpedantic -I./include
LDFLAGS= -lm -lpthread -lportaudio
PROJECT_NAME=digital_synth
SOURCE_FILES=$(wildcard src/*.c)

build: 
	$(CC) $(CFLAGS) $(LDFLAGS) $(SOURCE_FILES) main.c -o $(PROJECT_NAME)

clean:
	rm -f $(PROJECT_NAME)
