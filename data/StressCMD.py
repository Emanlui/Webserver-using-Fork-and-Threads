import threading
import sys
import os

# python3 StressCMD.py -n 5000 httpclient http://localhost/

def thread_function(input_host):
	command = "./httpclient " + input_host
	#print(command)
	os.system(command)
	

if __name__ == '__main__':

	print(sys.argv)

	counter = 0

	while(counter < int(sys.argv[2])):

		thread = threading.Thread(target=thread_function, args=(sys.argv[4],))
		thread.start()
		counter = counter + 1
