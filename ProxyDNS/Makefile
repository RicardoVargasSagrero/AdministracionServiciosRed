dns.o: proxy.c
	gcc -o dns.o $(mysql_config --cflags)proxy.c $(mysql_config --libs)

clean:
	rm *.o
	
run: dns.o
	./dns.o
