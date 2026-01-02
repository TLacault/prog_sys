LDFLAGS=-L.
LDLIBS=-lutils
CFLAGS+= -D_GNU_SOURCE -Wall -pthread

libutils.a: utils.o
	ar r $@ $?

clean:
	rm -f *~ *.o libutils.a
