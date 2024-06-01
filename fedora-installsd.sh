#!/bin/bash
# 	SD bash install script
# 	(c) 2023 Donald Montaine
#	This software is released under the Blue Oak Model License
#	a copy can be found on the web here: https://blueoakcouncil.org/license/1.0.0
#
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
tgroup=sdusers
tuser=$USER
cwd=$(pwd)
#
clear 
echo SD installer
echo --------------------
echo
echo "For this install script to work you must:"
echo
echo "  1 be running Fedora or a related distro"
echo
echo "  2 have sudo installed and be a member of the sudo group"
echo
echo
read -p "Continue? (y/n) " yn
case $yn in
	[yY] ) echo;;
	[nN] ) exit;;
	* ) exit ;;
esac
echo
echo If requested, enter your account password:
sudo date
echo
echo Installing required packages
echo
sudo dnf install make automake gcc gcc-c++ kernel-devel micro lynx libbsd-devel
 
cd sd64

# Create sd system user and group
echo "Creating group: sdusers"
sudo groupadd --system sdusers
sudo usermod -a -G sdusers root

echo "Creating user: sdsys."
sudo useradd --system sdsys -G sdusers

sudo cp -R sdsys /usr/local
sudo cp -R bin /usr/local/sdsys
sudo cp -R gplsrc /usr/local/sdsys
sudo cp -R gplobj /usr/local/sdsys
sudo cp -R terminfo /usr/local/sdsys
sudo cp Makefile /usr/local/sdsys
sudo cp gpl.src /usr/local/sdsys
sudo cp terminfo.src /usr/local/sdsys
sudo chown -R sdsys:sdusers /usr/local/sdsys
sudo chown root:root /usr/local/sdsys/ACCOUNTS/SDSYS
sudo chmod 664 /usr/local/sdsys/ACCOUNTS/SDSYS
sudo chown -R sdsys:sdusers /usr/local/sdsys/terminfo
sudo chown root:root /usr/local/sdsys
sudo cp sd.conf /etc/sd.conf
sudo chmod 644 /etc/sd.conf
sudo chmod -R 775 /usr/local/sdsys
sudo chmod 775 /usr/local/sdsys/bin
sudo chmod 775 /usr/local/sdsys/bin/*

#	Add $tuser to sdusers group
sudo usermod -aG sdusers $tuser

# directories for sd accounts
ACCT_PATH=/home/sd
if [ ! -d "$ACCT_PATH" ]; then
   sudo mkdir -p "$ACCT_PATH"/user_accounts
   sudo mkdir "$ACCT_PATH"/group_accounts
   sudo chown root:sdusers "$ACCT_PATH"/group_accounts
   sudo chmod 775 "$ACCT_PATH"/group_accounts
   sudo chown root:sdusers "$ACCT_PATH"/user_accounts
   sudo chmod 775 "$ACCT_PATH"/user_accounts
fi

sudo ln -s /usr/local/sdsys/bin/sd /usr/local/bin/sd

# Install sd service for systemd
SYSTEMDPATH=/usr/lib/systemd/system

if [ -d  "$SYSTEMDPATH" ]; then
    if [ -f "$SYSTEMDPATH/sd.service" ]; then
        echo "SD systemd service is already installed."
    else
		echo "Installing sd.service for systemd."

		sudo cp usr/lib/systemd/system/* $SYSTEMDPATH

		sudo chown root:root $SYSTEMDPATH/sd.service
		sudo chown root:root $SYSTEMDPATH/sdclient.socket
		sudo chown root:root $SYSTEMDPATH/sdclient@.service

		sudo chmod 644 $SYSTEMDPATH/sd.service
		sudo chmod 644 $SYSTEMDPATH/sdclient.socket
		sudo chmod 644 $SYSTEMDPATH/sdclient@.service

		sudo systemctl enable sd.service
		sudo systemctl enable sdclient.socket
    fi
fi

cd /usr/local/sdsys
sudo make -B

# Copy saved directories if they exist
if [ -d /home/sd/ACCOUNTS ]; then
  echo Moved existing ACCOUNTS directory
  sudo rm -fr /usr/local/sdsys/ACCOUNTS
  sudo mv /home/sd/ACCOUNTS /usr/local/sdsys
else
  echo Saved Accounts Directory Does Not Exist
fi

if [ -d '/home/sd/$LOGINS' ]; then
  echo 'Moved existing $LOGINS directory'
  sudo rm -fr '/usr/local/sdsys/$LOGINS'
  sudo mv '/home/sd/$LOGINS' /usr/local/sdsys
else
  echo 'Saved $LOGINS Directory Does Not Exist' 
fi

cd /usr/local/sdsys

#	Start SD server
echo "Starting SD server."
sudo bin/sd -start
echo
echo "Recompiling GPL.BP (only required for dev work)"
sudo bin/sd -internal FIRST.COMPILE
sudo bin/sd -internal SECOND.COMPILE

#  create a user account for the current user
echo
echo
if [ ! -d /home/sd/user_accounts/$tuser ]; then	
	echo "Creating a user account for" $tuser
	sudo bin/sd create-account USER $tuser no.query
fi

cd $cwd

# display end of script message
echo
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
echo
echo -----------------------------------------------------
echo
exit
