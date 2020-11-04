#include "httpd.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

struct stat st;
static int listenfd;
int *clients;
static void error(char *);
static void start_server(const char *);
static void respond(int);

int max_limit_of_clients;

static int clientfd;

static char *buf;

char webpage[] =

  "HTTP/1.1 200 OK\r\n"
  "Content-Type: text/html; charset=UTF-8\r\n\r\n"
  "<!DOCTYPE html>\r\n"
  "<html><head><title> Welcome to my server </title>\r\n"
  "<body><center><h1> This server was made with C </h1><br>\r\n"
  "<img src=\"dog.jpg\"></center></body/</html>\r\n";


void serve_forever(const char *PORT, int max_connections) {

  // Socket and client struct
  struct sockaddr_in clientaddr;
  socklen_t addrlen;

  int slot = 0;
  int fdimg;

  printf("Server started %shttp://127.0.0.1:%s%s\n", "\033[92m", PORT,
         "\033[0m");

  // The mmap() function is used for mapping between a process address space and either files or devices.
  // void * mmap (void *address, size_t length, int protect, int flags, int filedes,off_t offset)
  //https://linuxhint.com/using_mmap_function_linux/
  // create shared memory for client slot array
  clients = mmap(NULL, sizeof(*clients) * max_connections,
                 PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_SHARED, -1, 0);

  // Setting all elements to -1: signifies there is no client connected
  for (int i = 0; i < max_connections; i++)    clients[i] = -1;
    
  start_server(PORT);

  // ACCEPT connections
  while (1) {
  
    addrlen = sizeof(clientaddr);

    /**
    int accept(int socket, struct sockaddr *restrict address,socklen_t *restrict address_len);
    The accept() function shall extract the first connection on the queue of pending connections, create a new socket with the same socket type protocol and address family as the specified socket, and allocate a new file descriptor for that socket.
    socket       : Specifies a socket that was created with socket(), has been bound to an address with bind(), and has issued a successful call to listen().
    address      : Either a null pointer, or a pointer to a sockaddr structure where the address of the connecting socket shall be returned.
    address_len  : Points to a socklen_t structure which on input specifies the length of the supplied sockaddr structure, and on output specifies the length of the stor
    */

    clients[slot] = accept(listenfd, (struct sockaddr *)&clientaddr, &addrlen);

    if (clients[slot] < 0) {
      perror("accept() error");
      exit(1);
    } else {
      if (fork() == 0) {
        close(listenfd);
        respond(slot);
        //max_limit_of_clients++;
        //printf("MAX LIMIT: %d\n", max_limit_of_clients);
        
        //max_limit_of_clients--;

      
        //fdimg = open("/home/kali/Desktop/progra/data/bird.jpg", O_RDONLY);
        //sendfile(clients[slot], fdimg, NULL, 4000);

        close(fdimg);
        close(clients[slot]);
        exit(0);
      } else close(clients[slot]);
      
    }

    while (clients[slot] != -1){
      //printf("%d\n", slot);
      slot = (slot + 1) % max_connections;
      //printf("%d\n", slot);
      //printf("MAX LIMIT: %d\n", max_limit_of_clients);

    }

  }

}



// start server
void start_server(const char *port) {

  /**
 struct addrinfo {
   int              ai_flags;      This field specifies additional options, described below. Multiple flags are specified by bitwise OR-ing them together.
   int              ai_family;     This field specifies the desired address family for the returned addresses. 
   int              ai_socktype;   This field specifies the preferred socket type, for example SOCK_STREAM or SOCK_DGRAM.Specifying 0 in this field indicates that socket addresses of any type can be returned by getaddrinfo().
   int              ai_protocol;   This field specifies the protocol for the returned socket addresses.  Specifying 0 in this field indicates that socket addresses with any protocol can be returned by getaddrinfo().
   socklen_t        ai_addrlen;
   struct sockaddr *ai_addr;
   char            *ai_canonname;
   struct addrinfo *ai_next;
};
**/
  struct addrinfo hints, *res, *p;

  // https://www.tutorialspoint.com/c_standard_library/c_function_memset.htm
  // void *memset(void *str, int c, size_t n)
  // getaddrinfo for host
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
  // getaddrinfo() returns 0 if it succeeds
  // int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
  if (getaddrinfo(NULL, port, &hints, &res) != 0) {
    perror("getaddrinfo() error");
    exit(1);
  }

  // socket and bind
  for (p = res; p != NULL; p = p->ai_next) {
    int option = 1;

    // int socket(int domain, int type, int protocol);
    listenfd = socket(p->ai_family, p->ai_socktype, 0);
    // printf("SOCKET %d\n", listenfd);
    // int setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    if (listenfd == -1)
      continue;
    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    // When a socket is created with socket(2), it exists in a name space (address family) but has no address assigned to it.
    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
      break;
    
  }

  // In case there is no socket 
  if (p == NULL) {
    perror("socket() or bind()");
    exit(1);
  }

  freeaddrinfo(res);

  // listen for incoming connections
  /**
    
    listen() marks the socket referred to by sockfd as a passive socket,
    that is, as a socket that will be used to accept incoming connection
    requests using accept(2).
    The sockfd argument is a file descriptor that refers to a socket of
    type SOCK_STREAM or SOCK_SEQPACKET.
    The backlog argument defines the maximum length to which the queue of
    pending connections for sockfd may grow.  If a connection request
    arrives when the queue is full, the client may receive an error with
    an indication of ECONNREFUSED or, if the underlying protocol supports
    retransmission, the request may be ignored so that a later reattempt
    at connection succeeds.
  */
  if (listen(listenfd, 1000000) != 0) {
    perror("listen() error");
    exit(1);
  }
}

// get request header by name
char *request_header(const char *name) {
  header_t *h = reqhdr;
  //printf("HEADER: %s \n", h->name);
  //printf("HEADER: %s \n", h->value);
  while (h->name) {
    if (strcmp(h->name, name) == 0)
      return h->value;
    h++;
  }
  return NULL;
}

// get all request headers
header_t *request_headers(void) { return reqhdr; }


// This function is needed for my homework....
// This function is needed for my homework....
void print_protocol(char* method, char* uri ){

  if(!strcmp(input_protocol,"8080") || !strcmp(input_protocol,"80")){
    printf("It seems that you are using http \n");    
    fprintf(stderr, "\x1b[32m + [%s] %s\x1b[0m\n", method, uri);
  }else if(!strcmp(input_protocol,"21")){
    printf("It seems that you are using ftp \n");    
    fprintf(stderr, "\x1b[32m + [%s] \x1b[0m\n", method);
  }else if(!strcmp(input_protocol,"22")){
    printf("It seems that you are using ssh \n");    
    fprintf(stderr, "\x1b[32m + [%s] \x1b[0m\n", method);
  }else if(!strcmp(input_protocol,"25") || !strcmp(input_protocol,"587") || !strcmp(input_protocol,"465")){
    printf("It seems that you are using smtp \n");    
    fprintf(stderr, "\x1b[32m + [%s] \x1b[0m\n", method);
  } else if(!strcmp(input_protocol,"53")){
    printf("It seems that you are using dns \n");    
    fprintf(stderr, "\x1b[32m + [%s] \x1b[0m\n", method);
  } else if(!strcmp(input_protocol,"23")){
    printf("It seems that you are using telnet \n");    
    fprintf(stderr, "\x1b[32m + [%s] \x1b[0m\n", method);
  } else if(!strcmp(input_protocol,"123")){
    printf("It seems that you are using sntp \n");    
    fprintf(stderr, "\x1b[32m + [%s] \x1b[0m\n", method);
  }

  else printf("Unrecognized port: %s \n", input_protocol);

}


// client connection
void respond(int n) {
  int rcvd, fd, bytes_read;
  char *ptr;
  int buf_size = 65535;

  buf = malloc(buf_size);

  /**
      The recv(), recvfrom(), and recvmsg() calls are used to receive
       messages from a socket.  They may be used to receive data on both
       connectionless and connection-oriented sockets.  This page first
       describes common features of all three system calls, and then
       describes the differences between the calls.
       Return a ssize_t 
  */

  rcvd = recv(clients[n], buf, buf_size, 0);

  if (rcvd < 0) // receive error
    fprintf(stderr, ("recv() error\n"));
  else if (rcvd == 0) // receive socket closed
    fprintf(stderr, "Client disconnected upexpectedly.\n");
  else // message received
  {

    buf[rcvd] = '\0';

    int fdimg = 0;

    // char *strtok(char *str, const char *delim)
    method = strtok(buf, " \t\r\n");

    uri = strtok(NULL, " \t");
    
     
    //printf("URI: %s \n", uri);
    print_protocol(method, uri);

    // EQUALS
    if(!strncmp(uri,"/cgi-bin/", 8)){
      

        char * command = "";
        //memmove(uri, uri+1, strlen(uri));
        
        printf("URI %s\n", uri);
        strtok(uri, "/");
        char * program = strtok(NULL, "?");
        char * all_sintax_parameters = strtok(NULL, "\n");

        char* parameters_temp = "";
        char* token = "";
        token = strtok(all_sintax_parameters, "&");

        while(token != NULL){
          asprintf(&parameters_temp, "%s %s", parameters_temp, token);
          token = strtok(NULL, "&");
          //token = strtok(token, "=");
        }
        
        char *parameters = "";
        char* token2 = "";
        token2 = strtok(parameters_temp, "=");

        while(token2 != NULL){
          token2 = strtok(NULL, " ");
          asprintf(&parameters, "%s %s", parameters, token2);
          token2 = strtok(NULL, "=");
          //token = strtok(token, "=");
        }

        printf("PARAMETERS %s\n", parameters);
        printf("PROGRAM %s\n", program);
        printf("DIRECTORY %s\n", directory_location);
        asprintf(&command, "%s%s%s%s%s%s", "cd ", directory_location, "cgi-bin/ && ./", program, parameters, " > command.txt");
        
        system(command);
        printf("COMMAND %s\n", command);        

        char* text_location = "";
        asprintf(&text_location, "%s%s", directory_location, "cgi-bin/command.txt");

        FILE *command_file = fopen(text_location, "r");
        char c;
        char *command_data = "";

        while ((c = fgetc(command_file) )!= EOF){
              //printf ("%c", c);
              asprintf(&command_data, "%s%c", command_data, c);
        }

        printf("COMMAND OUTPUT %s\n", command_data);

        stat(text_location, &st);
        int size = st.st_size;

        write(clients[n], command_data, size);

    // DIFERENT
    }else if(strcmp(uri,"/")){

          char * img_path = NULL;
          memmove(uri, uri+1, strlen(uri));
          asprintf(&img_path, "%s%s", directory_location, uri);
         
          fdimg = open(img_path, O_RDONLY);

          stat(img_path, &st);
          int size = st.st_size;

          printf("%d\n", size);


          char image_header[] = "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/html; charset=UTF-8\r\n\r\n";



          write(clients[n], image_header, sizeof(image_header) - 1);  
          sendfile(clients[n], fdimg, NULL, size);


          close(fdimg);
 
      }else{

        
      
        // printf("METHOD: %s \n",method);
        // printf("URI: %s \n",uri);
        

        // char *strchr(const char *str, int c)
        // This returns a pointer to the first occurrence of the character c in the string str, or NULL if the character is not found.
        qs = strchr(uri, '?');

        // printf("%s\n", qs);

        if (qs) {
          *qs++ = '\0'; // split URI
        } else {
          qs = uri - 1; // use an empty string
        }

        header_t *h = reqhdr;
        char *t, *t2;
        while (h < reqhdr + 16) {
          char *k, *v, *t;

          k = strtok(NULL, "\r\n: \t");
          if (!k)
            break;

          v = strtok(NULL, "\r\n");
          while (*v && *v == ' ')
            v++;

          h->name = k;
          h->value = v;
          h++;
          fprintf(stderr, "[H] %s: %s\n", k, v);
          t = v + 1 + strlen(v);
          if (t[1] == '\r' && t[2] == '\n')
            break;
        }

        body = strtok(NULL, "\r\n");


        t++; // now the *t shall be the beginning of user payload
        t2 = request_header("Content-Length"); // and the related header if there is
        payload = qs;
        payload_size = t2 ? atol(t2) : (rcvd - (t - buf));

        // bind clientfd to stdout, making it easier to write
        clientfd = clients[n];

//        FILE *html_code;

        //char *html_directory = NULL;
        //asprintf(&html_directory, "%s%s", directory_location, "html/main");

        //html_code = fopen(html_directory, "r");

       
        //char c;
        //char * html_data = "";

        //while ((c = fgetc(html_code) )!= EOF){
        //      printf ("%c", c);
        //      asprintf(&html_data, "%s%c", html_data, c);
        //}

        //printf("CODE %s\n", html_data);

        //write(clients[n], html_data, 10000);  
        write(clients[n], webpage, sizeof(webpage) - 1);  
        //fclose(html_code);

        dup2(clientfd, STDOUT_FILENO);
        close(clientfd);

        // call router
        route();

        // tidy up
        fflush(stdout);
        shutdown(STDOUT_FILENO, SHUT_WR);
        close(STDOUT_FILENO);
      
      }
  }

  // Closing SOCKET
  shutdown(
      clientfd,
      SHUT_RDWR); // All further send and recieve operations are DISABLED...
  close(clientfd);
  clients[n] = -1;

  free(buf);
}

    