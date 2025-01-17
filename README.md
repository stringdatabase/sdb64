SD, the Multivalue String Database

SD is a multivalue database in the Pr1me Information tradition.  It contains open source code
from the Open Source databases openQM and ScarletDME and open source code developed by the
SD developers after the fork from ScarletDME.  While it shares many of the same features,
it was forked to explore some new ideas as to what a modern multivalue database should contain.

SD is 64 bit only and runs only on Linux.  Releases are tested on the most current release
of Debian, Ubuntu and Mint.  SD should run on any distribution based on Debian 12 
or Ubuntu 24.04

SD should cohabit peacefully with existing openQM and ScarletDME installations as
it is installed to a different location and uses memory offsets not used by OpenQM or ScarletDME.

The current version of the SD repository contains no binary bits.  Al features are available
for auditing.  Binary files are only created during the install.

To install on your Debian or Ubuntu based system, just clone the repository to the target computer
and then run the ubuntu-installsd.sh install script found in the sdb64 directory.

See the sd64/sdsys/changelog file for changes in each version release.
