import threading
import sys
import os


def thread_function(input_host, input_port):
	os.system('echo host: ' + input_host + ' port: ' + input_host)


if __name__ == '__main__':

	

	
	print(sys.argv)

	counter = 0

	while(counter < int(sys.argv[1])):

		input_host = sys.argv[2]
		input_port = sys.argv[3]
		thread = threading.Thread(target=thread_function, args=(input_host, input_port))
		thread.start()
		counter = counter + 1