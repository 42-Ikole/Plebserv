#! /usr/bin/python3

import subprocess
import glob


#
#
#
#

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

error_count = 0
total_count = 0

def print_message(message, result):
	global error_count
	global total_count
	total_count += 1
	if result == 1: 
		print('{:<40s} {:>20s}'.format(message, bcolors.OKGREEN + "[ OK ]" + bcolors.ENDC))
	else:
		print('{:<40s} {:>20s}'.format(message, bcolors.FAIL + "[ KO ]" + bcolors.ENDC))
		error_count += 1

def	send_command(curlcom, expected_status, to_compare=""):
	curl_split = curlcom.split()
	result = subprocess.run(curl_split, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	result_stio = result.stdout.decode("utf-8").replace('\r', '')
	header = result.stderr.decode("utf-8").split('\r\n')
	header_out = []
	header_in = []
	for i in header:
		if i.startswith('<'):
			header_in.append(i[2:])
		if i.startswith('>'):
			header_out.append(i[2:])
	print(header_in)

	if header_in[0].find(expected_status) == -1:
		return (print_message(curlcom, 0))
	if to_compare != "":
		f = open(to_compare, "r").read()
		if f != result_stio:
			return (print_message(curlcom, 0))
	return (print_message(curlcom, 1))

def	file_compare(loc1, loc2):
	f1 = open(loc1, 'r').read()
	f2 = open(loc2, 'r').read()
	return (f1 == f2)

def	main():
	send_command("curl -v -s http://localhost:5000/", "200", "html/Website/Welcome.html")
	send_command("curl -v -s http://localhost:5000/Store/", "200", "html/Website/Store/Start.html")
	send_command("curl -v -s http://localhost:5000/jajaja", "404", "")
	send_command("curl -X PUT --data-binary @/mnt/c/Users/Alpha_1337/Pictures/wp.png -v -s http://localhost:5000/new_image.png", "201", "") 
	# print_message("samecheck", file_compare("html/Websites/new_image.png", "/mnt/c/Users/Alpha_1337/Pictures/wp.png"))
	send_command("curl -X PUT --data-binary @/mnt/c/Users/Alpha_1337/Pictures/wp.png -v -s http://localhost:5000/new_image.png", "204", "")
	# print_message(file_compare("html/Websites/new_image.png", "/mnt/c/Users/Alpha_1337/Pictures/wp.png"))
	send_command("curl -X DELETE -v -s http://localhost:5000/new_image.png", "204", "")
	send_command("curl -X DELETE -v -s http://localhost:5000/new_image.png", "403", "")
	
	print(str(total_count - error_count) + " out of " + str(total_count) + " OK")
	if (error_count):
		exit(1)
	exit(0)


if __name__ == "__main__":
    main()