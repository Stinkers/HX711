CC=gcc
CFLAGS=-I.
OBJ = rpio.o kuage.o hx711.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

kuage: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~
