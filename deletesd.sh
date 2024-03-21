#!/bin/bash
#
# 	SD bash delete script
# 	(c) 2023 Donald Montaine
#	This software is released under the Blue Oak Model License
#	a copy can be found on the web here: https://blueoakcouncil.org/license/1.0.0
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
echo REMOVE SD Completely
echo ------------------------------------
	read -p "Are you sure? (y/n) " yn
	case $yn in
		[yY] ) echo;;
		[nN] ) break;;
		* ) break;;
	esac
# remove the /usr/sdsys directory
sudo rm -fr /usr/local/sdsys
echo
echo "Removed /usr/local/sdsys directory."
# remove the symbolic link to sd in /usr/local/bin
sudo rm /usr/local/bin/sd
echo "Removed symbolic link /usr/local/bin/sd."
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
echo "Do you want to delete /home/sd and all subdirectories."
echo "WARNING: this will delete all SD User and Group accounts."
echo 
read -p "Delete all user and group  accounts? (y/n) " yn
case $yn in
	[yY] ) sudo rm -fr /home/sd && echo "User and group accounts removed";;
	[nN] ) ;;
	* ) exit ;;
esac
echo
echo "----------------------------------------------"
echo "deletesd.sh script completed."
echo "Reboot to update user and group information."
echo "----------------------------------------------"
echo

