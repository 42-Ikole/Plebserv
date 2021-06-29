#! /usr/bin/python3

import subprocess
import glob
import filecmp


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

def	send_command(curlcom, expected_status, to_compare="", check_header = ""):
	curl_split = curlcom.split()
	result = subprocess.run(curl_split, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
	try:
		result_stio = result.stdout.decode("utf-8").replace('\r', '')
		header = result.stderr.decode("utf-8").split('\n')
	except:
		return print_message(curlcom, 1)
	header_out = []
	header_in = []
	for i in header:
		if i.startswith("< "):
			header_in.append(i[2:])
		if i.startswith("> "):
			header_out.append(i[2:])

	if header_in[0].find(expected_status) == -1:
		return (print_message(curlcom, 0))

	if check_header != "":
		if (check_header in header_in == False):
			return (print_message(curlcom, 0))
	if to_compare != "":
		f = open(to_compare, "r").read()
		if f != result_stio:
			return (print_message(curlcom, 0))
	return (print_message(curlcom, 1))

def	file_compare(loc1, loc2):
	return (filecmp.cmp(loc1, loc2))

def	main():
	send_command("curl -v -s http://localhost:5000/", "200", "html/Website/Welcome.html")
	send_command("curl -v -s http://localhost:5000/Store/", "200", "html/Website/Store/Start.html")
	send_command("curl -v -s http://localhost:5000/jajaja", "404", "")
	send_command("curl -v -s http://localhost:5000/redir", "301", "", "Location: /Coin/Home.html")
	send_command("curl -X OPTIONS -v -s http://localhost:5000/", "204", "", "Allow: GET, POST, OPTIONS")
	send_command("curl -v -s -X PUT -H \"Content-Type: application/json\" -d \"{\"key1\":\"value\"}\" http://localhost:5000/test.json", "405", "")
	send_command("curl -v -s -X PUT -H \"Content-Type: application/json\" -d \"{\"key1\":\"value\"}\" http://localhost:5000/upload/test.json", "201", "")
	send_command("curl -v -s -X PUT -H \"Content-Type: application/json\" -d \"{\"key1\":\"value\"}\" http://localhost:5000/upload/test.json", "204", "")
	send_command("curl -v -s -X PUT -H \"Content-Type: application/json\" -d \"{\"key1\":\"valueaaaaaaaaaaaaaa\"}\" http://localhost:5000/upload/test.json", "413", "")
	send_command("curl -v -s -X PUT -H \"Content-Type: application/json\" -d \"{\"key1\":\"value\"}\" http://localhost:5000/upload/", "409", "")
	send_command("curl -X DELETE -v -s http://localhost:5000/upload/test.json", "405", "")
	send_command("curl -X DELETE -v -s http://localhost:5000/delete/test.json", "204", "")
	send_command("curl -X DELETE -v -s http://localhost:5000/delete/test.json", "403", "")
	send_command("curl -v -s http://localhost:5000/cgis/best_lang.sh", "200", "")
	send_command("curl -v -s http://localhost:5000/cgis/hello.py", "200", "", "X-secret-kiss: haram")
	send_command("curl -v -s http://localhost:5000/cgis/status.py", "69", "")
	send_command("curl -X POST -d \"HAHAHHAHA\" -v -s http://localhost:5000/cgis/postme.rb", "200", "")
	send_command("curl -v -X POST --data-binary @./plebserv.png http://localhost:5000/images/test1.png", "200", "")
	send_command("curl -v -X POST --data-binary @./plebserv.png -H \"Transfer-Encoding: chunked\" http://localhost:5000/images/test2.png", "200", "")
	print(file_compare("plebserv.png", "html/Website/uploads/test1.png"))
	print(file_compare("plebserv.png", "html/Website/uploads/test2.png"))
	
	print(str(total_count - error_count) + " out of " + str(total_count) + " OK")
	if (error_count):
		exit(1)
	exit(0)


if __name__ == "__main__":
    main()