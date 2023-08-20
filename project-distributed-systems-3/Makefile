objects = libclaves.so cliente servidor
CC = gcc
CFLAGS = -Wall
LD = ld
LD_LIBRARY_PATH = $LD_LIBRARY_PATH:.
export LD_LIBRARY_PATH

all: $(objects)

libclaves.so: claves.c claves.h tuplas_clnt.c tuplas_xdr.c
	$(CC) $(CFLAGS) -fPIC -o claves.o claves.c -c 
	$(LD) -lrt -o libclaves.so claves.o -shared 
	rm claves.o

cliente: cliente.c claves.h	tuplas_clnt.c tuplas_xdr.c
	$(CC) $(CFLAGS) -o cliente cliente.c -L. -lclaves

servidor: servidor.c servidor.h proxy.c	tuplas_svc.c tuplas_xdr.c
	$(CC) $(CFLAGS) -o servidor servidor.c proxy.c -lpthread

clean:
	rm $(objects)