#! /usr/bin/python3

import subprocess
import glob


#
#	TODO:
#	- log file
#	- check if parsed correctly
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

f = open("log.txt", "w")

def test(path_to_config):
	result = subprocess.run(['./plebserv', '-t', path_to_config], stdout=subprocess.PIPE)
	# res = result.stdout.decode('utf-8').split("\n")
	return (result.returncode)

def main():
	onlyfiles = glob.glob("configs/*.conf")
	count = 0
	error_count = 0
	result = 0
	for i in onlyfiles:
		if (i.find('wr') != -1):
			result = test(i) - 1
		else:
			result = test(i)
		if (result == 0):
			print(i + "\t\t" + bcolors.OKGREEN + "[ OK ]" + bcolors.ENDC)
		else:
			print(i + "\t\t" + bcolors.FAIL + "[ KO ]" + bcolors.ENDC)
			error_count += 1
		count += 1
	print(str(count - error_count) + " out of " + str(count) + " OK")
	if (error_count):
		exit(1)
	exit(0)


if __name__ == "__main__":
    main()