# Webserver in C


[![Build Status](https://travis-ci.org/joemccann/dillinger.svg?branch=master)](https://github.com/Emanlui/DDOS)

This repo is a Webserver that can do the following things:

  - Can detect the Method from the client.
  - Open images/video from the specify directory.
  - Execute binaries from the cgi-bin.
  - There are two scripts that can download resources from any url.
  - A DOS attack using those scripts.
  - Create two different servers, with Threads and with Fork.
  - It can detect FTP, SSH, SMTP, DNS, TELNET, SNMP petitions.

# Dependencies

You need to install curl before running the program

```sh
        $ sudo apt update
        $ sudo apt upgrade
        $ sudo apt install curl
```
 
# How to run it!

  - For the server, you just need to run:
```sh
        $ make
        $ ./prethread-webserver -n <max-connections> -w <path-www-root> -p <port>
 ```
  - For the client:
```sh
        $ gcc -L/usr/lib/x86_64-linux-gnu httpclient.c -o httpclient -lcurl
 ```      

# Things to keep in mind

In case you run into a problem of permissions, you need to run the server with sudo

# Examples 

### Clients

```sh
        $ ./httpclient [http://url/resource.jpg] 
        $ python3 httpclient.py [http://url/resource.jpg] 
 ```
 
> These scripts only download the resources  

### Stress

```sh
        $ python3 stress.py -n <how-many-threads> httpclient <parameter of the httpclient>
 ```
 
> This script run the command N times, this is the simulation of the DOS

### Server

This server can be from the pre-thread side or the pre-forked. 

```sh
        $ ./prethread-webserver -n <max-connections> -w <path-www-root> -p <port>
```

> The server will take the path of the www and do a find for all the files inside this directory, you have to be
> carefull if the directory has a bad file distribution.
> The server can run binaries and print it to the screen, you have to go to:

```sh
        $ http://localhost/cgi-bin/{path-to-binary}?param1=a&param2=b
```

> You can try to see the responses of the server by doing:

```sh
        $ curl -X [things like POST/GET/PATCH/PUT/...] http://localhost/
        $ ssh {user}@localhost -p22
```


