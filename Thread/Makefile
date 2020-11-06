all: prethread-webserver

clean:
	@rm -rf *.o
	@rm -rf prethread-webserver

prethread-webserver: main.o httpd.o
	gcc -pthread -o prethread-webserver $^ 

main.o: main.c httpd.h
	gcc -c -o main.o main.c

httpd.o: httpd.c httpd.h
	gcc -pthread -c -o httpd.o httpd.c

