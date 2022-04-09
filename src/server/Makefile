OBJ = miniserver.o
LIBS = -lpthread
miniserver : $(OBJ)
	cc -o miniserver $(OBJ) $(LIBS)
miniserver.o : miniserver.h minitcp.h

.PHONY : all clean
all :
	make miniserver
clean :
	rm -f *.o