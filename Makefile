CC = gcc
CFLAGS = -Wall -Wextra -ggdb -std=c11 -O3 -DVIDEO # -DINTERACTIVE

WIN_WIDTH  = 800
WIN_HEIGHT = 800
WIN_FRAMES = 1200

SRC=$(wildcard *.c)
OBJ=$(patsubst %.c, %.o, $(SRC))

EXE=gfx.exe

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lm

interactive.o: interactive.c
	$(CC) $(CFLAGS) -c -o $@ $^

main.o: main.c
	$(CC) $(CFLAGS) -D WIN_WIDTH=$(WIN_WIDTH) -D WIN_HEIGHT=$(WIN_HEIGHT) -D WIN_FRAMES=$(WIN_FRAMES) -c -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(OBJ) $(EXE) *.bmp *.mp4

genvid: $(EXE)
	./gfx.exe | ffmpeg -y -f rawvideo -pixel_format rgb32 -video_size $(WIN_WIDTH)x$(WIN_HEIGHT)\
		-framerate 60 -i - -vf "transpose=1, transpose=1" -c:v libx264 -profile:v baseline -level 3.0 -pix_fmt yuv420p\
		video.mp4; ffplay video.mp4
