#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>
#include <string.h>

 
int main(int argc, char **argv)
{
 
 	// ./httpclient https://upload.wikimedia.org/wikipedia/commons/4/42/Tetrao_urogallus_Richard_Bartz.jpg photo.jpg
 	// gcc -L/usr/lib/x86_64-linux-gnu httpclient.c -o httpclient -lcurl


	//if (argc != 3) {    fprintf(stderr, "USAGE: ./httpclient <hostname> <port>\n"); return 1;  }	
 
 	//char* url = argv[1];
 	//char* port = argv[2];
 	//printf(url);
 	//printf(port);

 	//char * complete_url = (char *) malloc(1 + strlen(url)+ strlen(port) );
    //strcpy(complete_url, url);
    //strcat(complete_url, port);
    //printf("%s", complete_url);

    // This variable is in charge of all operations
	CURL *curl_handle;

	// File pointer
	FILE *fp;

	fp = fopen("DownloadedFile.jpg", "wb");

	 /* init the curl session */ 
  	curl_handle = curl_easy_init();
	 
  	/* set URL to get here -- CURLOPT_URL is needed for the GET */ 
  	curl_easy_setopt(curl_handle, CURLOPT_URL, argv[1]);
	 
  	/* send all data to this function  */ 
  	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, fp);


  	curl_easy_setopt(curl_handle, CURLOPT_FAILONERROR, 1L);

  	int result = curl_easy_perform(curl_handle);
	 

  	printf("%d", result);
  	if(result == CURLE_OK) printf("The file was succesfullly downloaded");
  	else printf("Error downloading the file %s", curl_easy_strerror(result));


    fclose(fp);

  	/* cleanup curl stuff */ 
  	curl_easy_cleanup(curl_handle);
	 
  	


  	return 0;
}