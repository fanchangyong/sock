sock:sock.o
	cc -o sock sock.o

sock.o:sock.c
	cc -c sock.c
