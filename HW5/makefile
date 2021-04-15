BIN = parser
CC = g++

SSRCS = ourgetopt.cpp  printtree.cpp  semantic.cpp  symbolTable.cpp  yyerror.cpp
SRCS = $(BIN).y  $(BIN).l  $(SSRCS)
HDRS = scanType.h  ourgetopt.h  printtree.h  semantic.h  symbolTable.h  yyerror.h
OBJS = lex.yy.o $(BIN).tab.o

c- : $(OBJS)
	$(CC) -g $(OBJS) $(SSRCS) -o c-

lex.yy.c : $(BIN).l $(BIN).tab.h $(HDR)
	flex $(BIN).l

$(BIN).tab.h $(BIN).tab.c : $(BIN).y
	bison -v -t -d $(BIN).y

clean :
	rm -f *~ $(OBJS) $(BIN) lex.yy.c $(BIN).tab.h $(BIN).tab.c $(BIN).output

tar : $(HDRS) $(SRCS) makefile
	tar -cvf c-.tar $(HDRS) $(SRCS) makefile