from tqdm import tqdm
import requests
import sys
import os

def download_file(url):

	# 'http://localhost:7777?archive=photo.jpg'

    response = requests.get(url, stream=True)

    with open("download_file", "wb") as handle:
        for data in tqdm(response.iter_content()):
            handle.write(data)

if __name__ == '__main__':

	download_file(sys.argv[1])