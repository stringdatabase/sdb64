SD, the Multivalue String Database

SD is a multivalue database in the Pr1me Information tradition.  It contains open source code
from the Open Source databases OpenQM and ScarletDME.  While it shares many of the same features,
it was forked to explore some new ideas as to what a modern multivalue database should contain.

SD is 64 bit only and runs only on Linux.  Releases are tested on the most current release
of Linux Mint LMDE (Debian based).  It should run on any Ubuntu flavor 22.04 or later, and 
probably on other distros derived from Debian 12 or from Ubuntu 22.04 or later. Other distros, YMMV.

As far as I can tell, SD will cohabit peacefully with existing QM and ScarletDME installations as
it is installed to a different location and uses memory offsets not used by OpenQM or ScarletDME.

See the sd64/sdsys/changelog file for changes in each version release.
