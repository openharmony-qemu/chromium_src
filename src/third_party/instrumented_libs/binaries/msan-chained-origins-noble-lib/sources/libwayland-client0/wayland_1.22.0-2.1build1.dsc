-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 1.0
Source: wayland
Binary: libwayland-client0, libwayland-egl1, libwayland-server0, libwayland-cursor0, libwayland-dev, libwayland-egl-backend-dev, libwayland-doc, libwayland-bin
Architecture: any all
Version: 1.22.0-2.1build1
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Emilio Pozuelo Monfort <pochu@debian.org>, Héctor Orón Martínez <zumbi@debian.org>, Marius Gripsgard <mariogrip@debian.org>
Homepage: https://wayland.freedesktop.org/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/xorg-team/wayland/wayland
Vcs-Git: https://salsa.debian.org/xorg-team/wayland/wayland.git
Build-Depends: debhelper (>= 10), quilt, pkg-config, libexpat1-dev, libffi-dev, libxml2-dev, libwayland-bin <cross>, meson
Build-Depends-Indep: doxygen, graphviz, xmlto, xsltproc, docbook-xsl
Package-List:
 libwayland-bin deb libdevel optional arch=any
 libwayland-client0 deb libs optional arch=any
 libwayland-cursor0 deb libs optional arch=any
 libwayland-dev deb libdevel optional arch=any
 libwayland-doc deb doc optional arch=all
 libwayland-egl-backend-dev deb libdevel optional arch=any
 libwayland-egl1 deb libs optional arch=any
 libwayland-server0 deb libs optional arch=any
Checksums-Sha1:
 4cde2cb4569842dabdd37865c93e4b2f0ef6428f 362983 wayland_1.22.0.orig.tar.gz
 bdcf4ab1447904fe93212af10725e94798aabe2c 16367 wayland_1.22.0-2.1build1.diff.gz
Checksums-Sha256:
 b1b023a31902286e476e109ed48070a815d3ab5f615daec29e9635188881eece 362983 wayland_1.22.0.orig.tar.gz
 3a22976ff3f1d80b9c7bdebcf0667f2ca4b30f3c2feaaa7e358b2e307a7d082e 16367 wayland_1.22.0-2.1build1.diff.gz
Files:
 e9ca0026a4ddb862b09015fab36eb650 362983 wayland_1.22.0.orig.tar.gz
 2057f7ee93592ca183b8d46616cb4092 16367 wayland_1.22.0-2.1build1.diff.gz
Original-Maintainer: Debian X Strike Force <debian-x@lists.debian.org>

-----BEGIN PGP SIGNATURE-----

iQJHBAEBCgAxFiEET7WIqEwt3nmnTHeHb6RY3R2wP3EFAmYUGdsTHGp1bGlhbmtA
dWJ1bnR1LmNvbQAKCRBvpFjdHbA/cQNmD/9+D3lJeV9AChEk94VWs6rGdzxxKbBE
LtRd4w/BN0xVCHCRzNKouDpSWH9VgZho1ntz2GF5//+n/A7lHnTeVO45rpPtU1jk
EsrQAFOBfgpDdCMg+JxJkAJlwdDfKvSAbTmeu7OIYQJIV2sGe9ECYDWUdYLqcbVR
3X83f27g1zIOOuvRg69gHDQSrP4N7D7J3IV8jNDjEvOM3Pe7Pxll+5H9njHarY4u
f9ZOGKbe4y5grEih/dcfAI3df1fKHXLLfjRhZIrn9ZoN0ezWkuFAGFxjVWUSqjYH
Kjk27j52Yo7GQdOGQe0Cq3U6usA9F/4fiUQnLa5ggSLOP6YphOpDadDMtZWGyXhc
S7uNSzmHrUCnpfCU2FabCvrRxB/rvfFuapypWYs8UA69Ixvpst7XvH1RrlzEfo7s
ttChfYTt0a45L1Wm4Yvu6Qyu2JVwrJVF0kQUkj6RewW1rND+XZ19yBv3zOXF8Wdj
TGT97azjyX1KaWVxqCP4bhikeer1S2ByYmYdVk5AAQk6vhIosoiaMD+Z3itOVddr
2poYDMo0twQ5XO3Mq/89KEjC7a/hG2tWhRTnWBHTPdSAlXmV4fRTFp3BZLKfKJqE
ED4GwcmefNOYIkjkkrZXuxsxk7LCKNXvJAMsi6yNaSxphVHuxnHBmdL92kwJq0du
TOPp/M0VYD9bAg==
=fi72
-----END PGP SIGNATURE-----
