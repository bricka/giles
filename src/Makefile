#CFLAGS += -DDEBUG -g

giles: giles.o cddb.o ui.o rip.o misc.o
	$(CC) $(CFLAGS) -Wall -o giles giles.o cddb.o ui.o rip.o misc.o -lcddb -lcdio `pkg-config --cflags --libs gtk+-3.0`

ui.o: ui.c
	$(CC) $(CFLAGS) -Wall `pkg-config --cflags gtk+-3.0` -c ui.c

clean:
	rm -rf giles *.o

.PHONY: clean
