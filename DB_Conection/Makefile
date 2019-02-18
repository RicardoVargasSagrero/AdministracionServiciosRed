build: conection.c 
	gcc -o output.o $(mysql_config --cflags)conection.c $(mysql_config --libs)

clean:
	rm *.o

run: connection.o
	./connection.o
