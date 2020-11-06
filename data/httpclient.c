#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>
#include <string.h>

 
int main(int argc, char **argv)
{
 
  // ./httpclient https://upload.wikimedia.org/wikipedia/commons/4/42/Tetrao_urogallus_Richard_Bartz.jpg photo.jpg
  // gcc -L/usr/lib/x86_64-linux-gnu httpclient.c -o httpclient -lcurl

  // This variable is in charge of all operations
  CURL *curl_handle;

  // File pointer
  FILE *fp;

  fp = fopen("DownloadedFile", "wb");

   /* init the curl session */ 
    curl_handle = curl_easy_init();
   
    /* set URL to get here -- CURLOPT_URL is needed for the GET */ 
    curl_easy_setopt(curl_handle, CURLOPT_URL, argv[1]);
   
    /* send all data to this function  */ 
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, fp);


    curl_easy_setopt(curl_handle, CURLOPT_FAILONERROR, 1L);

    int result = curl_easy_perform(curl_handle);
   
    if(result== CURLE_OK) printf("The file was succesfullly downloaded \n");
    else printf("Error downloading the file %s \n", curl_easy_strerror(result));


    fclose(fp);

    /* cleanup curl stuff */ 
    curl_easy_cleanup(curl_handle);
   
    

    return 0;
}