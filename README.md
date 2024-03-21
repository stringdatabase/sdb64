SD, the Multivalue String Database

The changelog can be found in the sd64/sdsys directory

Links
-----

SD is based on the GPL Version of OpenQM v2.6.6, released in 2007 by Ladybridge Systems.
The original OpenQM code as of it's release in 2007 can be found on github in the 
dmontaine/openqm repository. It is never updaated.

ScarletDME is a 32 and 64 bit fork of the original OpenQM database maintained by Greg Buckle
The ScarletDME code from which SD was forked (as of 19 Jan 2024) is stored as it was forked 
and is never updated in the dmontaine/ScarletDME/tree/dev repository.

The code repository for SD is also on github in the dmontaine/sd repository.
SD is 64 bit only and runs only on Linux.  Releases are tested on the most current release
of Xubuntu.  It should run on any Ubuntu flavor 22.04 or later, and probably on other distros derived
from Debian 12 orfrom Ubuntu 22.04 or later. Other distros, YMMV.

The current release snapshot can be found on PCloud.
The download link is: https://u.pcloud.link/publink/show?code=kZL58V0Z9wXWbRq9r88aPtNBpMe4rh9drHgV

As far as I can tell, SD will cohabit peacefully with existing QM and ScarletDME installations as
it is installed to a different location and uses memory offsets not used by OpenQM or ScarletDME.

See the sd64/sdsys/changelog file for changes in each version release.
