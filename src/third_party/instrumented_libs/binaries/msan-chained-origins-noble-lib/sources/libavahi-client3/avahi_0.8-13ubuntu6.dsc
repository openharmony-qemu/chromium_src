-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: avahi
Binary: avahi-daemon, avahi-dnsconfd, avahi-autoipd, python3-avahi, avahi-utils, avahi-discover, libavahi-common3, libavahi-common-data, libavahi-common-dev, libavahi-core7, libavahi-core-dev, libavahi-client3, libavahi-client-dev, libavahi-glib1, libavahi-glib-dev, libavahi-gobject0, libavahi-gobject-dev, libavahi-compat-libdnssd1, libavahi-compat-libdnssd-dev, libavahi-ui-gtk3-0, libavahi-ui-gtk3-dev, avahi-ui-utils, gir1.2-avahi-0.6
Architecture: any all
Version: 0.8-13ubuntu6
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Sjoerd Simons <sjoerd@debian.org>, Sebastian Dröge <slomo@debian.org>, Loic Minier <lool@dooz.org>, Michael Biebl <biebl@debian.org>
Homepage: https://avahi.org/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/utopia-team/avahi
Vcs-Git: https://salsa.debian.org/utopia-team/avahi.git
Testsuite: autopkgtest
Testsuite-Triggers: build-essential, pkgconf
Build-Depends: debhelper (>= 13.11.6~), debhelper-compat (= 13), dh-sequence-gir <!nogir>, dh-sequence-python3, intltool, libcap-dev [linux-any], libdaemon-dev, libdbus-1-dev, libexpat-dev, libgdbm-dev, libgirepository1.0-dev <!nogir>, libglib2.0-dev, libgtk-3-dev <!pkg.avahi.nogui>, pkgconf, python-gi-dev <!nopython>, python3, python3-dbus <!nopython>, python3-gdbm, python3-gi <!nopython>, xmltoman
Package-List:
 avahi-autoipd deb net optional arch=linux-any
 avahi-daemon deb net optional arch=any
 avahi-discover deb net optional arch=all profile=!nopython,!pkg.avahi.nogui
 avahi-dnsconfd deb net optional arch=any
 avahi-ui-utils deb utils optional arch=any profile=!pkg.avahi.nogui
 avahi-utils deb net optional arch=any
 gir1.2-avahi-0.6 deb introspection optional arch=any profile=!nogir
 libavahi-client-dev deb libdevel optional arch=any
 libavahi-client3 deb libs optional arch=any
 libavahi-common-data deb libs optional arch=any
 libavahi-common-dev deb libdevel optional arch=any
 libavahi-common3 deb libs optional arch=any
 libavahi-compat-libdnssd-dev deb libdevel optional arch=any
 libavahi-compat-libdnssd1 deb libs optional arch=any
 libavahi-core-dev deb libdevel optional arch=any
 libavahi-core7 deb libs optional arch=any
 libavahi-glib-dev deb libdevel optional arch=any
 libavahi-glib1 deb libs optional arch=any
 libavahi-gobject-dev deb libdevel optional arch=any profile=!nogir
 libavahi-gobject0 deb libs optional arch=any
 libavahi-ui-gtk3-0 deb libs optional arch=any profile=!pkg.avahi.nogui
 libavahi-ui-gtk3-dev deb libdevel optional arch=any profile=!pkg.avahi.nogui
 python3-avahi deb python optional arch=any profile=!nopython
Checksums-Sha1:
 969a50ae18c8d8e2288435a75666dd076e69852a 1591458 avahi_0.8.orig.tar.gz
 3664c95e3b1d12e68e9c51e9d5dbd80aa8225f95 49216 avahi_0.8-13ubuntu6.debian.tar.xz
Checksums-Sha256:
 060309d7a333d38d951bc27598c677af1796934dbd98e1024e7ad8de798fedda 1591458 avahi_0.8.orig.tar.gz
 deabd9cb8bc9a6a1b80de80bb0471d965f719a5aef3dadf87c5b9ecf74b5e4b8 49216 avahi_0.8-13ubuntu6.debian.tar.xz
Files:
 229c6aa30674fc43c202b22c5f8c2be7 1591458 avahi_0.8.orig.tar.gz
 82344ddf7fa7fecfbd8a5d484e4de27c 49216 avahi_0.8-13ubuntu6.debian.tar.xz
Original-Maintainer: Utopia Maintenance Team <pkg-utopia-maintainers@lists.alioth.debian.org>

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCgAdFiEEpi0s+9ULm1vzYNVLFZ61xO/Id0wFAmYP3LwACgkQFZ61xO/I
d0wzAw/5Adym4+ymn5cHlyPYK+ybKiZZOk3DergqlAQ59X9WNx2mxYotQtk/nRJS
cCJ7eDwqV6WYVUQKypqT6gbsn0M2aWe2+kyOgDEXaZXg24DuGgN1w/Hn8cAsvNH4
BA5o6fved5qhzIc3P3+L262Cu/C4uhUFzW/JIY0vT0P2a/z8kvGdJ5aIQj72a2vj
quDnt2yaKWv0mkek9T+PWuadILzS9c5Zxgjz0Kd9K3mcfJvGbYx8XY1wjrlFvmm2
Rh2UdxYLK/21jn0/xvMbpU4jlO7J5JR95JKJ8KFcHe/Ih/aMA220yrPyrEXUfish
iCuUpw6y/+BvVgUQoc1hYCoqQ/6AG9LHimVqls3U4fvTH3eE/N0XjNNM5L0RsbWT
YEmTFRKBOoO4iKrv+tvVzaeU2jbaHdWjsRtNOnv4HP2hwl52BcgiRr3SdnwHhupg
91UamRRQ2Jq8gOwqRG+H9HM2yjvfeQkzaJNgsGwuaNY2aP0lf6LpnpYw9HtFbo73
mjY98tQMBVBKd2S55DCGVoRLoy9q7/1bmfg0ukyP+oIgx1YZU2/fIis8L5FjLfbP
sc3wD71I7VwfyrjNK+w/sF8H67HZHdK4ZykAuyHqe3GYApXdM4Tg/d0Z0gqc34mN
ViMsIhDle6DYGUfxbI021g6zb0APIqen7uK9uFHEQPED4FjInIs=
=nQvv
-----END PGP SIGNATURE-----
