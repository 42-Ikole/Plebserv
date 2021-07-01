#! /bin/bash

echo "Installing tester.."

mkdir html

OS_NAME=$(uname -s)

echo $OS_NAME

if [ "$OS_NAME" = "Linux" ]; then
	echo "Downloading cgi.."
	wget https://projects.intra.42.fr/uploads/document/document/4015/ubuntu_cgi_tester -q -O cgi_tester

	echo "Downloading tester.."
	wget https://projects.intra.42.fr/uploads/document/document/4017/ubuntu_tester -q -O tester
elif [ $OS_NAME = "Darwin" ]; then
	echo "Downloading cgi.."
	wget https://projects.intra.42.fr/uploads/document/document/4016/cgi_tester -q -O cgi_tester

	echo "Downloading tester.."
	wget https://projects.intra.42.fr/uploads/document/document/4014/tester -q -O tester
else
	echo "error downloading"
fi

echo "Creating files.."

mkdir html/YoupiBanane
cd html/YoupiBanane

touch youpi.bad_extension
touch youpi.bla
mkdir nop
mkdir Yeah

cd nop
touch youpi.bad_extension
touch other.pouic

cd ../Yeah
touch not_happy.bad_extension

cd ../../../

chmod 777 tester
