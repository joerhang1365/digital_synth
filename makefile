CC=gcc
CFLAGS= -g -O0 -std=c99 -Wall -Werror -Wpedantic
LDFLAGS= -lm -lportaudio
PROJECT_NAME=digital_synth

build: 
	$(CC) $(CFLAGS) $(LDFLAGS) main.c -o $(PROJECT_NAME)

clean:
	rm -f $(PROJECT_NAME)
