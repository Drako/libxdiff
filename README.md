libxdiff
========

This is a fork of LibXDiff by Davide Libenzi.
I basically threw away the GNU autotools based build system
and made some minor adjustments to some effected headers.

So here it is. Currently it requires CMake > 2.8.11,
but I am not sure if it would also work with lower versions.

Original project can be found here: http://www.xmailserver.org/xdiff-lib.html

The fork is based on the version 0.23 source tarball.

I also converted the library to a shared library (so/dll) so the license is actually useful...
LGPL with a static library is kind of not useful.

LICENSE
=======

LGPL version 2.1 (See COPYING or http://www.gnu.org/copyleft/lesser.html)

