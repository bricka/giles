#CFLAGS += -DDEBUG -g

giles: giles.o cddb.o ui.o
	$(CC) $(CFLAGS) -Wall -o giles giles.o cddb.o ui.o -lcddb -lcdio `pkg-config --cflags --libs gtk+-3.0`

giles.o: giles.c
	$(CC) $(CFLAGS) -Wall `pkg-config --cflags --libs gtk+-3.0` -c giles.c

ui.o: ui.c
	$(CC) $(CFLAGS) -Wall `pkg-config --cflags --libs gtk+-3.0` -c ui.c

clean:
	rm -rf giles *.o

.PHONY: clean
