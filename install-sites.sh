#! /bin/bash

HOST='89.99.93.83'
USER='guest'
PASSWD='webserv'
FILE='Website.tar'

mkdir html

echo "Downloading phpmyadmin.."
wget https://files.phpmyadmin.net/phpMyAdmin/5.0.1/phpMyAdmin-5.0.1-english.tar.gz -q -O html/phpmyadmin.tar.gz
echo "Downloading wordpress.."
wget https://nl.wordpress.org/latest-nl_NL.tar.gz -q -O html/wordpress.tar.gz

cd html

echo "Downloading custom html site.."
ftp -n $HOST <<END_SCRIPT
pass
quote USER $USER
quote PASS $PASSWD
binary
get $FILE
quit
END_SCRIPT

echo "Extracting phpmyadmin.."
tar xf phpMyAdmin.tar.gz
mv phpMyAdmin-5.0.1-english phpmyadmin

echo "Extracting wordpress.."
tar xf wordpress.tar.gz

echo "Extracting custom html site.."
tar xf Website.tar

echo "Cleaning up.."
rm -rf phpMyAdmin.tar.gz
rm -rf wordpress.tar.gz
rm -rf Website.tar

# tar xf 