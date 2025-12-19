SD, the Multivalue String Database

SD is a multivalue database in the Pr1me Information tradition.  It contains open source code
from the Open Source databases openQM and ScarletDME and open source code developed by the
SD developers after the fork from ScarletDME.  While it shares many of the same features,
it was forked to explore some new ideas as to what a modern multivalue database should contain.

SD is 64 bit only and runs only on Linux.  Releases are tested on the most current release
of Debian, Ubuntu, Mint and Fedora.  SD should run on any distribution based on Debian 12,
Ubuntu 24.04 or Fedora 41.  The installer has also installed the database successfully on Ubuntu running
under the Windows Subsystem for Linux, and Ubuntu on the Raspberry Pi 5.

SD should cohabit peacefully with existing openQM and ScarletDME installations as it
creates and uses a System V shared memory segment that will not conflict with OpenQM or ScarletDME.

The current version of the SD repository contains no binary bits.  All features are available
for auditing.  Binary files are only created during the install.

To install on one of the supported systems, use the installsd.sh script found at
https://github.com/stringdatabase/sd-scripts/archive/refs/heads/main.zip .

To install SD, just download and extract the scripts, make the scripts executable if needed and then run installsd.sh. The installer will handle installation of required packages, downloading and compiling of source, installation of SD into the proper directories and deletion of the temporary files. 

See the sd64/sdsys/changelog file for changes in each version release.
