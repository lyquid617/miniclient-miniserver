OBJ = miniclient.o userinterface.o
LIBS = `pkg-config --cflags --libs gtk+-3.0`

miniclient : $(OBJ)
	cc -o miniclient $(OBJ) $(LIBS) -D ONEHUNDRED
miniclient.o :miniclient.c miniclient.h 
	cc -c miniclient.c -D ONEHUNDRED
userinterface.o : userinterface.c userinterface.h
	cc -c userinterface.c $(LIBS) -D ONEHUNDRED

.PHONY : all clean
all :
	make miniclient
clean :
	rm -f *.o