#include "httpd.h"
#include <stdlib.h>

int main(int c, char **argv) {

  int connection = (int)atoi(argv[2]);
  directory_location = argv[4];
  serve_forever(argv[6], connection);
  return 0;
}

void route() {
  ROUTE_START()

  ROUTE_GET("/") {
    printf("HTTP/1.1 200 OK\r\n\r\n");
    printf("Hello! You are using %s", request_header("User-Agent"));
  }

  ROUTE_POST("/") {
    printf("HTTP/1.1 200 OK\r\n\r\n");
    printf("Wow, seems that you POSTed %d bytes. \r\n", payload_size);
    printf("The payload was: %s \r\n", payload);
    printf("Fetch the data using `payload` variable. \r\n");
    printf("The directory location is: %s \r\n", directory_location);
    printf("The body is: %s \r\n", body);
  }


  ROUTE_PUT("/") {
    printf("HTTP/1.1 200 OK\r\n\r\n");
  }

  ROUTE_DELETE("/") {
    printf("HTTP/1.1 200 OK\r\n\r\n");
  }

  ROUTE_UPDATE("/") {
    printf("HTTP/1.1 200 OK\r\n\r\n");
  }


  ROUTE_END()
}
