CC=cmoc
RM=rm -rf
SAPFS=./sapfs
ASTYLE=astyle

CFLAGS= -O2 --thomto

LDFLAGS= -lcmoc-crt-thm


all: lz77dec.o tofloppy.o loader floppy

lz77dec.o: lz77dec.c
	$(CC) $(CFLAGS) -c $<
	$(CC) -S $(CFLAGS) $<

tofloppy.o: tofloppy.c
	$(CC) $(CFLAGS) -c $<
	$(CC) -S $(CFLAGS) $<

loader: lz77dec.o tofloppy.o loader.c
	$(CC) $(CFLAGS) -o$@.bin  $^ $(LDFLAGS)
	$(CC) -S $(CFLAGS)  $^


floppy:
	dd if=/dev/zero of=test.ttt bs=1 count=6953
	$(SAPFS) -c floppy.sap
	$(SAPFS) -a floppy.sap *.bin
	$(SAPFS) -a floppy.sap moby-d00.167
	$(SAPFS) -a floppy.sap fouAPi00.167
	$(SAPFS) -a floppy.sap beast000.167
	$(SAPFS) -a floppy.sap arton400.167
	$(SAPFS) -a floppy.sap arton500.167
	$(SAPFS) -a floppy.sap Makefile
	$(SAPFS) -t floppy.sap

format:
	$(ASTYLE) --style=linux *.c

clean:
	$(RM) *.o *.a *.bin
