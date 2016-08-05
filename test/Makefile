CC = gcc
CFLAGS = -Wall

MKDIR_P = mkdir -p
SRCDIR = ../src
SDKDIR = ../src/sdk
OBJDIR = ../obj
BINDIR = ../bin
INCDIR = ../includes

all: test

debug: CFLAGS += -g3 -D DEBUG
debug: test

test: directories RS232.o genericcommands.o commands.o configreader.o $(INCDIR)/protocolconf.h functions.o test.o
	gcc $(CFLAGS) $(OBJDIR)/RS232.o $(OBJDIR)/genericcommands.o $(OBJDIR)/commands.o $(OBJDIR)/configreader.o $(OBJDIR)/functions.o $(OBJDIR)/test.o -o $(BINDIR)/test

RS232.o: $(SDKDIR)/RS232.c $(SDKDIR)/RS232.h
	gcc -c $(CFLAGS) $(SDKDIR)/RS232.c -o $(OBJDIR)/RS232.o
	
genericcommands.o: $(SDKDIR)/genericcommands.c $(SDKDIR)/genericcommands.h $(INCDIR)/protocolconf.h
	gcc -c $(CFLAGS) $(SDKDIR)/genericcommands.c -o $(OBJDIR)/genericcommands.o
	
commands.o: $(SRCDIR)/commands.c $(SRCDIR)/commands.h $(INCDIR)/protocolconf.h
	gcc -c $(CFLAGS) $(SRCDIR)/commands.c -o $(OBJDIR)/commands.o
	
configreader.o: $(SRCDIR)/configreader.c $(SRCDIR)/configreader.h
	gcc -c $(CFLAGS) $(SRCDIR)/configreader.c -o $(OBJDIR)/configreader.o

functions.o: functions.c
	gcc -c $(CFLAGS) functions.c -o $(OBJDIR)/functions.o

test.o: test.c
	gcc -c $(CFLAGS) test.c -o $(OBJDIR)/test.o

directories:
	$(MKDIR_P) $(OBJDIR)
	$(MKDIR_P) $(BINDIR)

clean-all: clean
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

clean: clean-inc clean-src clean-sdk
	rm -f $(OBJDIR)/*.o *~ $(BINDIR)/test

clean-inc:
	rm -f $(INCDIR)/*~;

clean-src:
	rm -f $(SRCDIR)/*~;
	
clean-sdk:
	rm -f $(SDKDIR)/*~;
