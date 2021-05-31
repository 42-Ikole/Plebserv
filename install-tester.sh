#! /bin/bash

echo "Installing tester.."

mkdir html

echo "Installing cgi.."
wget https://projects.intra.42.fr/uploads/document/document/3408/ubuntu_cgi_tester -q -O html/cgi-tester

echo "Installing tester.."
wget https://projects.intra.42.fr/uploads/document/document/3410/ubuntu_tester -q -O tester

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

sh -c "./plebserv configs/42-tester.conf" &
printf "\n\n\n\n\n" | sh -c "./tester http://localhost:5000"