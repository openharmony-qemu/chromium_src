-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: dbus
Binary: dbus, dbus-session-bus-common, dbus-system-bus-common, dbus-bin, dbus-daemon, dbus-1-doc, dbus-tests, dbus-udeb, dbus-user-session, dbus-x11, libdbus-1-3, libdbus-1-3-udeb, libdbus-1-dev
Architecture: any all
Version: 1.14.10-4ubuntu4
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders:  Sjoerd Simons <sjoerd@debian.org>, Sebastian Dröge <slomo@debian.org>, Michael Biebl <biebl@debian.org>, Loic Minier <lool@dooz.org>, Simon McVittie <smcv@debian.org>,
Homepage: https://dbus.freedesktop.org/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/utopia-team/dbus
Vcs-Git: https://salsa.debian.org/utopia-team/dbus.git
Testsuite: autopkgtest
Testsuite-Triggers: apparmor, build-essential, gnome-desktop-testing, init, systemd, xauth, xvfb
Build-Depends: autoconf-archive, automake, debhelper (>= 13.11.6~), debhelper-compat (= 13), dh-exec, libapparmor-dev [linux-any], libaudit-dev [linux-any], libcap-ng-dev [linux-any], libexpat-dev, libglib2.0-dev <!nocheck> <!noinsttest>, libnss-wrapper <!nocheck>, libselinux1-dev [linux-any], libsystemd-dev [linux-any], libx11-dev, pkgconf, valgrind-if-available, xml-core, xmlto <!nodoc>
Build-Depends-Indep: doxygen <!nodoc>, ducktype <!nodoc>, xsltproc <!nodoc>, yelp-tools <!nodoc>
Package-List:
 dbus deb admin standard arch=any
 dbus-1-doc deb doc optional arch=all profile=!nodoc
 dbus-bin deb admin optional arch=any
 dbus-daemon deb admin optional arch=any
 dbus-session-bus-common deb admin optional arch=all
 dbus-system-bus-common deb admin optional arch=all
 dbus-tests deb misc optional arch=any profile=!noinsttest
 dbus-udeb udeb debian-installer optional arch=any profile=!noudeb
 dbus-user-session deb admin optional arch=linux-any
 dbus-x11 deb x11 optional arch=any
 libdbus-1-3 deb libs optional arch=any
 libdbus-1-3-udeb udeb debian-installer optional arch=any profile=!noudeb
 libdbus-1-dev deb libdevel optional arch=any
Checksums-Sha1:
 47f03306a491509cdebacded837e51b086835252 1372328 dbus_1.14.10.orig.tar.xz
 76219dcec55c66705eb1a311d11ced2df190758d 833 dbus_1.14.10.orig.tar.xz.asc
 156f56a186fa1270de86305b660a84ecd48abe5e 69608 dbus_1.14.10-4ubuntu4.debian.tar.xz
Checksums-Sha256:
 ba1f21d2bd9d339da2d4aa8780c09df32fea87998b73da24f49ab9df1e36a50f 1372328 dbus_1.14.10.orig.tar.xz
 5f292cd0603c3d736026ed3f4d1c1937847981669c1f0a389083518f013e1081 833 dbus_1.14.10.orig.tar.xz.asc
 0844907258be5dc2dccf41ec3ff80fc3baf5bde9a11f9072299e24ae29ab6721 69608 dbus_1.14.10-4ubuntu4.debian.tar.xz
Files:
 46070a3487817ff690981f8cd2ba9376 1372328 dbus_1.14.10.orig.tar.xz
 a790dcc5fd84835ed9147c9679af551f 833 dbus_1.14.10.orig.tar.xz.asc
 926fe5418520620e84659379eb8db24a 69608 dbus_1.14.10-4ubuntu4.debian.tar.xz
Original-Maintainer: Utopia Maintenance Team <pkg-utopia-maintainers@lists.alioth.debian.org>

-----BEGIN PGP SIGNATURE-----

iQJHBAEBCgAxFiEET7WIqEwt3nmnTHeHb6RY3R2wP3EFAmYUAYgTHGp1bGlhbmtA
dWJ1bnR1LmNvbQAKCRBvpFjdHbA/cVBED/9Iuzijc9WiJfihNp52ajA/EGqzZKq8
MC+KhQGz4JNw/DPE8ux3XfPPwiFYufh73x9Zi0Mkr03xoffKcm75NjdLfjEoGxf5
vEX1Xa9sTs9h9Rjf+hEOQZXg6Fv6UCSE4t84/LVf893h1Eo+vTBYPi6cTKBRXA46
u9edkJ9diVfPTv0ZYh+8mPderx0hd7wcHjSvp8x+BFcydPXZ0MYNCVQ22RxUR4lh
J8VF7L6iobAAbcBfrPQr52Aj+lMXmN50Gl787XC1rzIryZ1lEVyjnLzRu7jidZHc
HhPad3Iz5xszJKixWF6J8EMKvh8uSB6APODF05c1lWIz7pDaZW12aAr8ehT8+Fzq
v9WIfwJGG71tLrSVD8ngqLt4WhEuzPf+4sv6HGUrhbbbQeux2uzdzwXYDirTdskR
ftiyobPHgynxgS73H9xcnmvYXK/eZr6liDUz40OVlDZIZTtDE7NWL47Ul1EkIQPp
swnCFVdPMZEQF679LhoWn7I+RGS/b8utQgAGSj82kpisoJvSDMGgrDm6nTSX29yS
/GbxOzCdb50yqZXXCyF7JWgKWowvIgedPPJ6TQAYIDyOOVI7zCe9zyQbHHIs1pwI
7GpErdpidahRB9h02omcu/zobhMB/v4zUCi6phf+icl/s5wCbJ5q3bY7klmc3Y/q
nmTevp2fqzmH+g==
=sMPU
-----END PGP SIGNATURE-----
