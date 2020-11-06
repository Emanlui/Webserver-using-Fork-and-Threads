all: preforked-webserver

clean:
	@rm -rf *.o
	@rm -rf preforked-webserver

preforked-webserver: main.o httpd.o
	gcc -o preforked-webserver $^

main.o: main.c httpd.h
	gcc -c -o main.o main.c

httpd.o: httpd.c httpd.h
	gcc -c -o httpd.o httpd.c

