BIN = parser
CC = g++

SRCS = $(BIN).y  $(BIN).l
HDRS = scanType.h
OBJS = lex.yy.o $(BIN).tab.o

c- : $(OBJS)
	$(CC) $(OBJS) -o c-

lex.yy.c : $(BIN).l $(BIN).tab.h $(HDR)
	flex $(BIN).l

$(BIN).tab.h $(BIN).tab.c : $(BIN).y
	bison -v -t -d $(BIN).y

clean :
	rm -f *~ $(OBJS) $(BIN) lex.yy.c $(BIN).tab.h $(BIN).tab.c $(BIN).output

tar : $(HDR) $(SRCS) makefile
	tar -cvf c-.tar $(HDRS) $(SRCS) makefile