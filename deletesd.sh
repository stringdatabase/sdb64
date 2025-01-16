#!/bin/bash
#
# 	SD bash delete script
# 	(c) 2023 Donald Montaine
#	This software is released under the Blue Oak Model License
#	a copy can be found on the web here: https://blueoakcouncil.org/license/1.0.0
#
#
    if [[ $EUID -eq 0 ]]; then
       echo "This script must NOT be run as root" 1>&2
       exit
    fi
    if [ -f  "/usr/local/sdsys/bin/sd" ]; then
		echo
	else
		echo "SD is not installed!"
		echo "This script will not run."
		exit
	fi
#
clear
echo If requested, enter your account password:
sudo pwd
echo
echo
echo REMOVE SD Completely
echo ------------------------------------
echo
echo "Do you want to save /home/sd and all subdirectories."
echo
echo "WARNING: Choose 'R' to retain accounts for SD reinstallation."
echo "         Choose 'D' will delete all SD User and Group accounts."
echo 
echo

read -p "Delete /home/sd? (R/d) " rd
case $rd in
	[dD] )  echo /home/sd Directory Deleted;
			sudo rm -fr /home/sd;;
	* )  
		    echo Saving Accounts Directory;
		    sudo mv /usr/local/sdsys/ACCOUNTS /home/sd;
esac
echo

# remove the /usr/sdsys directory
sudo rm -fr /usr/local/sdsys
echo
echo "Removed /usr/local/sdsys directory."

# remove the symbolic link to sd in /usr/local/bin or /usr/bin
if [ -L "/usr/local/bin/sd" ]; then
	sudo rm /usr/local/bin/sd
	echo "Removed symbolic link /usr/local/bin/sd."
fi

if [ -L "/usr/bin/sd" ]; then
	sudo rm /usr/bin/sd
	echo "Removed symbolic link /usr/bin/sd."
fi

#remove config file
sudo rm /etc/sd.conf
echo "Config file removed"
#
cd /usr/lib/systemd/system

#stop services
sudo systemctl stop sd.service
sudo systemctl stop sdclient.socket

# disable services
sudo systemctl disable sd.service
sudo systemctl disable sdclient.socket

# remove service files
sudo rm /usr/lib/systemd/system/sd.service
sudo rm /usr/lib/systemd/system/sdclient.socket
sudo rm /usr/lib/systemd/system/sdclient@.service

echo "Removed systemd service files."
# remove sdsys user and sdusers group
sudo userdel sdsys
sudo groupdel sdusers
echo "Removed sdsys user and sdusers group."

echo
echo "----------------------------------------------"
echo "deletesd.sh script completed."
echo "Reboot to update user and group information."
echo "----------------------------------------------"
echo
read -p "Restart computer now? (y/N) " yn
case $yn in
	[yY] ) sudo reboot;;
	[nN] ) echo;;
	* ) echo ;;
esac
echo
