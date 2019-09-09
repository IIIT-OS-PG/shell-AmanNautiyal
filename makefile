CC=gcc
DEPS = custom_headers.h

%.o: %.c ${DEPS}
	$(CC) -c -o $@ $< $(CFLAGS)

PosixShell: PosixShell.o Initializer.o Parser.o Executor.o
	$(CC) -o PosixShell PosixShell.o Initializer.o Parser.o Executor.o
