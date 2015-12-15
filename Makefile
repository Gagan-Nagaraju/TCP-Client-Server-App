

CC = gcc

LIBS = -lresolv -lsocket -lnsl -lpthread\
	/home/courses/cse533/Stevens/unpv13e_solaris2.10/libunp.a\
	
FLAGS = -g -O2

CFLAGS = ${FLAGS} -I/home/courses/cse533/Stevens/unpv13e_solaris2.10/lib

all: echotimecli timecli_final echocli_final echosrv_thread

# clients

echotimecli: echotimecli.o
	${CC} ${FLAGS} -o echotimecli echotimecli.o ${LIBS}
echotimecli.o: echotimecli.c
	${CC} ${CFLAGS} -c echotimecli.c

timecli_final: timecli_final.o
	${CC} ${FLAGS} -o timecli_final timecli_final.o ${LIBS}
timecli_final.o: timecli_final.c
	${CC} ${CFLAGS} -c timecli_final.c

echocli_final: echocli_final.o
	${CC} ${FLAGS} -o echocli_final echocli_final.o ${LIBS}
echocli_final.o: echocli_final.c
	${CC} ${CFLAGS} -c echocli_final.c




# server using the thread-safe version of readline.c

echosrv_thread: echosrv_thread.o readline.o
	${CC} ${FLAGS} -o echosrv_thread echosrv_thread.o readline.o ${LIBS}
echosrv_thread.o: echosrv_thread.c
	${CC} ${CFLAGS} -c echosrv_thread.c




# thread-safe version of readline.c is picked up from the directory "threads"

readline.o: /home/courses/cse533/Stevens/unpv13e_solaris2.10/threads/readline.c
	${CC} ${CFLAGS} -c /home/courses/cse533/Stevens/unpv13e_solaris2.10/threads/readline.c


clean:
	rm echotimecli echotimecli.o timecli_final timecli_final.o echocli_final echocli_final.o echosrv_thread echosrv_thread.o readline.o
