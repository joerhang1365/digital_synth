CC=$(PREFIX)gcc

CFLAGS = -Wall -Werror=implicit-function-declaration
CFLAGS += -I. -I/usr/local/include

OBJS = \
	   assert.o \
	   error.o \
	   midi.o \
	   oscillator.o \

LDFLAGS = -L/usr/local/lib -lportaudio -lpthread -lm

all: synth

synth: $(OBJS) main.o
	$(CC) $(OBJS) main.o $(LDFLAGS) -o $@

clean:
	rm -rf *.o synth
