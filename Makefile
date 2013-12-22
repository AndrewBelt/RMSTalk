CFLAGS = -g -Wall --std=c99 -O0 \
	-I/usr/local/include

LDFLAGS = \
	-L/usr/local/lib \
	-lportaudio \
	-lvorbis \
	-lvorbisenc \
	-logg

OBJS = \
	src/rmstalk.c.o \
	src/main.c.o \
	src/vorbis.c.o \
	src/audio.c.o

all: rmstalk

rmstalk: $(OBJS)
	$(LINK.c) -o $@ $^

%.c.o: %.c
	$(COMPILE.c) -o $@ $^

clean:
	rm -f rmstalk $(OBJS)
