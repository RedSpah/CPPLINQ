CC = g++
CFLAGS = -I.
DEPS = cpplinq.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cpplinqmake: testing.cpp
	$(CC) -o test testing.o -I.
