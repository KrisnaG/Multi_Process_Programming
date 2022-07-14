# ./parallel_search_keyspace 5 B1AF2507B69F11CCB3AE2C3592039 assignment_cipher.txt plain.txt

COMPILER = gcc
CFLAGS = -Wall -pedantic -g
COBJS = ring_util.o aes_util.o
CEXES = generate_ciphertext decrypt_ciphertext parallel_search_keyspace

all: ${CEXES}

generate_ciphertext: generate_ciphertext.c
	${COMPILER} ${CFLAGS} generate_ciphertext.c -lcrypto -o generate_ciphertext -Wall

decrypt_ciphertext: decrypt_ciphertext.c
	${COMPILER} ${CFLAGS} decrypt_ciphertext.c -lcrypto -o decrypt_ciphertext -Wall

parallel_search_keyspace: parallel_search_keyspace.c ${COBJS}
	${COMPILER} ${CFLAGS} parallel_search_keyspace.c ${COBJS} -lcrypto  -o parallel_search_keyspace

%.o: %.c %.h  makefile
	${COMPILER} ${CFLAGS} $< -c

clean:
	rm -f *.o *~ ${CEXES}