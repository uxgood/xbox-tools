CC=gcc
all: wtd

wtd: wtd.c
	$(CC) $< -o $@

