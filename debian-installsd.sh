#!/bin/bash
# 	SD bash install script
# 	(c) 2023 Donald Montaine
#	This software is released under the Blue Oak Model License
#	a copy can be found on the web here: https://blueoakcouncil.org/license/1.0.0
#
#------------------------------------------------------------------
# CHECK IF ALREADY INSTALLED
#------------------------------------------------------------------
    if [[ $EUID -eq 0 ]]; then
        echo "This script must NOT be run as root" 1>&2
        exit
    fi
    if [ -f  "/usr/local/sdsys/bin/sd" ]; then
		echo "A version of sd is already installed"
		echo "Uninstall it before running this script"
		exit
	fi
#
#------------------------------------------------------------------	
# VARIABLE DEFINITIONS
#------------------------------------------------------------------
ACCT_PATH=/home/sd
CWD=$(pwd)
SDSYS_PATH=/usr/local/sdsys
SYSTEMD_PATH=/usr/lib/systemd/system
TGROUP=sdusers
TUSER=$USER

#------------------------------------------------------------------
# STARTUP USER MESSAGEE
#------------------------------------------------------------------
clear 
echo SD installer
echo --------------------
echo
echo "For this install script to work you must:"
echo
echo "  1 be running a distro based on Debian 12 or Ubuntu 22.04 or later"
echo
echo "  2 have sudo installed and be a member of the sudo group"
echo
read -p "Continue? (y/N) " yn
case $yn in
	[yY] ) echo;;
	[nN] ) exit;;
	* ) exit ;;
esac

#------------------------------------------------------------------
# INSTALL REQUIRED DEB PACKAGES
#------------------------------------------------------------------
echo
echo "If requested, enter your account password:"
sudo pwd
echo
echo Installing required packages
echo
sudo apt-get install build-essential micro lynx libbsd-dev python3

#------------------------------------------------------------------
# INSTALL SYSTEM USER AND GROUP
#------------------------------------------------------------------
echo
echo "Creating group: sdusers"
sudo groupadd --system sdusers
sudo usermod -a -G sdusers root

echo "Creating user: sdsys."
sudo useradd --system sdsys -G sdusers

#------------------------------------------------------------------
# COPY FILES & DIRECTORIES TO SDSYS DIRECTORY & SET RIGHTS
#------------------------------------------------------------------
echo
echo "Setting up sdsys directory tree and permissions"
sudo cp -R $CWD/sd64/sdsys /usr/local
sudo cp -R $CWD/sd64/bin $SDSYS_PATH
sudo cp -R $CWD/sd64/gplsrc $SDSYS_PATH
sudo cp -R $CWD/sd64/gplobj $SDSYS_PATH
sudo cp -R $CWD/sd64/terminfo $SDSYS_PATH
sudo cp $CWD/sd64/Makefile $SDSYS_PATH
sudo cp $CWD/sd64/gpl.src $SDSYS_PATH
sudo cp $CWD/sd64/terminfo.src $SDSYS_PATH
sudo cp $CWD/sd64/sd.conf /etc/sd.conf

#set rights
sudo chown -R sdsys:sdusers $SDSYS_PATH
sudo chown root:root $SDSYS_PATH/ACCOUNTS/SDSYS
sudo chmod 664 $SDSYS_PATH/ACCOUNTS/SDSYS
sudo chown -R sdsys:sdusers $SDSYS_PATH/terminfo
sudo chown root:root $SDSYS_PATH
sudo chmod 644 /etc/sd.conf
sudo chmod -R 775 $SDSYS_PATH
sudo chmod 775 $SDSYS_PATH/bin
sudo chmod 775 $SDSYS_PATH/bin/*

#------------------------------------------------------------------
# SETUP THE USER AND GROUP ACCOUNT BASE DIRECTORIES
# OWNERSHIP & RIGHTS
#------------------------------------------------------------------
echo
echo "Setup user and group accounts tree"
if [ ! -d "$ACCT_PATH" ]; then
   sudo mkdir -p "$ACCT_PATH"/user_accounts
   sudo mkdir "$ACCT_PATH"/group_accounts
   sudo chown root:sdusers "$ACCT_PATH"/group_accounts
   sudo chmod 775 "$ACCT_PATH"/group_accounts
   sudo chown root:sdusers "$ACCT_PATH"/user_accounts
   sudo chmod 775 "$ACCT_PATH"/user_accounts
fi

#------------------------------------------------------------------
# MAKE THE EXECUTABLES AND LINK TO SYSTEM BINARY DIRECTORY
#------------------------------------------------------------------
echo
echo "Build executables" 
cd $CWD/sd64
sudo make -B
cd $CWD

# create link to sd executable
sudo ln -s $SDSYS_PATH/bin/sd /usr/local/bin/sd

#------------------------------------------------------------------
# COPY THE ACCOUNTS DIRECTORY IF SAVED DURING LAST DELETION
#------------------------------------------------------------------
echo
echo "Restore the accounts directory tree if saved during last deletion"
if [ -d /home/sd/ACCOUNTS ]; then
  echo Moved existing ACCOUNTS directory
  sudo rm -fr $SDSYS_PATH/ACCOUNTS
  sudo mv /home/sd/ACCOUNTS $SDSYS_PATH
else
  echo Saved Accounts Directory Does Not Exist
fi

#------------------------------------------------------------------
# START SD TO COMPILE ALL THE BASIC PROGRAMS IN GPL.BP
#------------------------------------------------------------------
echo
echo "Run SD server to compile programs in GPL.BP"
sudo $SDSYS_PATH/bin/sd -start
echo
echo "Recompiling GPL.BP (only required for dev work)"
sudo $SDSYS_PATH/bin/sd -internal FIRST.COMPILE
sudo $SDSYS_PATH/bin/sd -internal SECOND.COMPILE
echo

#------------------------------------------------------------------
# ADD CURRENT USER TO sdusers AND CREATE USER ACCOUNT
#------------------------------------------------------------------
echo
echo "Add current user to sdusers and create database account"
sudo usermod -aG sdusers $TUSER

#  create a database user account for the current user
echo
if [ ! -d /home/sd/user_accounts/$TUSER ]; then	
	echo "Creating a user account for" $TUSER
	sudo $SDSYS_PATH/bin/sd create-account USER $TUSER no.query
fi

#------------------------------------------------------------------
# STOP SD AND INSTALL SD SERVICES
#------------------------------------------------------------------
echo
echo "Stop sd server and install services for systemd"
sudo $SDSYS_PATH/bin/sd -stop
echo
if [ -d  "$SYSTEMD_PATH" ]; then
    if [ -f "$SYSTEMD_PATH/sd.service" ]; then
    	echo
        echo "SD systemd service is already installed."
    else
    	echo
		echo "Installing sd.service for systemd."
		sudo cp $CWD/sd64/usr/lib/systemd/system/* $SYSTEMD_PATH

		sudo chown root:root $SYSTEMD_PATH/sd.service
		sudo chown root:root $SYSTEMD_PATH/sdclient.socket
		sudo chown root:root $SYSTEMD_PATH/sdclient@.service

		sudo chmod 644 $SYSTEMD_PATH/sd.service
		sudo chmod 644 $SYSTEMD_PATH/sdclient.socket
		sudo chmod 644 $SYSTEMD_PATH/sdclient@.service
		
 		echo Enabling services
		sudo systemctl start sd.service
		sudo systemctl start sdclient.socket
		sudo systemctl enable sd.service
		sudo systemctl enable sdclient.socket
    fi
fi
#------------------------------------------------------------------
# DISPLAY END OF SCRIPT MESSAGE AND REBOOT QUESTION
#------------------------------------------------------------------
echo
echo -----------------------------------------------------
echo "The SD server is installed."
echo
echo "The /home/sd directory has been created."
echo "User directories are created under /home/sd/user_accounts."
echo "Group directories are created under /home/sd/group_accounts."
echo "Accounts are only created using CREATE-ACCOUNT in SD."
echo
echo "Reboot to assure that group memberships are updated"
echo "and the APIsrvr Service is enabled."
echo
echo "After rebooting, open a terminal and enter 'sd' "
echo "to connect to your sd home directory."
echo
echo "To completely delete SD, run the" 
echo "deletesd.sh bash script provided."
echo -----------------------------------------------------
echo
read -p "Restart computer now? (y/N) " yn
case $yn in
	[yY] ) sudo reboot;;
	[nN] ) echo;;
	* ) echo ;;
esac
exit
