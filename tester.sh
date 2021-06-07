#! bin/bash

kernel=$(uname)

GREEN="\x1b[32;01m"
RED="\x1b[31;01m"
RESET="\x1b[0m"
if [ "$kernel" = "Linux" ]; then
	GREEN="\033[32;01m"
	RED="\033[31;01m"
	RESET="\033[0m"
fi

FILE=plebserv
if [ ! -f "$FILE" ]; then
	echo "Please make $FILE. ✗"
else
	echo "Starting testing. ✓"
fi

# ./plebserv &
# MY_PID=$!

# sleep 2

send_and_compare()
{
	eval $1 > test.tmp
	A=$(diff test.tmp $2)

	echo -n "Test $3:	{"
	if [ "$A" != "" ]
	then
		echo -en "${RED}Error"
	else
		echo -en "${GREEN}Success"
	fi

	echo -e "${RESET}}"

}
send_and_compare "curl -s http://localhost:5000/" html/Website/Welcome.html 1
send_and_compare "curl -s --http1.0 -v http://localhost:5000/" html/Website/Welcome.html 2
# send_and_compare "curl -s -v -X POST -F 'name=linuxize' -F 'email=linuxize@example.com' http://localhost:5000/test.php" 

# kill $MY_PID
