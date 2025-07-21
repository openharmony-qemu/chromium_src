-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: fontconfig
Binary: fontconfig, fontconfig-config, fontconfig-udeb, libfontconfig-dev, libfontconfig1-dev, libfontconfig1, libfontconfig-doc
Architecture: any all
Version: 2.15.0-1.1ubuntu2
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Keith Packard <keithp@debian.org>, Emilio Pozuelo Monfort <pochu@debian.org>,
Homepage: https://www.freedesktop.org/wiki/Software/fontconfig/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/freedesktop-team/fontconfig
Vcs-Git: https://salsa.debian.org/freedesktop-team/fontconfig.git
Build-Depends: debhelper-compat (= 13), libfreetype-dev (>= 2.8.1), libexpat1-dev, uuid-dev, pkgconf, python3:any, gperf, po-debconf
Build-Depends-Indep: docbook <!nodoc>, docbook-utils <!nodoc>, texlive-formats-extra <!nodoc>
Package-List:
 fontconfig deb fonts optional arch=any
 fontconfig-config deb fonts optional arch=any
 fontconfig-udeb udeb debian-installer optional arch=any profile=!noudeb
 libfontconfig-dev deb libdevel optional arch=any
 libfontconfig-doc deb doc optional arch=all profile=!nodoc
 libfontconfig1 deb libs optional arch=any
 libfontconfig1-dev deb oldlibs optional arch=any
Checksums-Sha1:
 b6137ee5d542c0fe5c96a7724884f2e8e212d275 1447820 fontconfig_2.15.0.orig.tar.xz
 4b3d06fb33dda2abc8392caedd045eafa7b1e296 30556 fontconfig_2.15.0-1.1ubuntu2.debian.tar.xz
Checksums-Sha256:
 63a0658d0e06e0fa886106452b58ef04f21f58202ea02a94c39de0d3335d7c0e 1447820 fontconfig_2.15.0.orig.tar.xz
 29ebacc2cd8f2d336150b3895feea8bb35e4340f7a34dfb37d915be76fb36b91 30556 fontconfig_2.15.0-1.1ubuntu2.debian.tar.xz
Files:
 5bb3a2829aecb22ae553c39099bd0d6a 1447820 fontconfig_2.15.0.orig.tar.xz
 0cd21516902a26e0660df5a7ff382e4a 30556 fontconfig_2.15.0-1.1ubuntu2.debian.tar.xz
Original-Maintainer: Debian freedesktop.org maintainers <pkg-freedesktop-maintainers@lists.alioth.debian.org>

-----BEGIN PGP SIGNATURE-----

iQJOBAEBCgA4FiEErEg/aN5yj0PyIC/KVo0w8yGyEz0FAmYItXcaHHN0ZXZlLmxh
bmdhc2VrQHVidW50dS5jb20ACgkQVo0w8yGyEz3aOQ/+IOas6oyr6PBj4qcuwkGa
FsNcdqSNAsYtikA+ax0zASh/qh3HgB5AbUfq/CuDSXdc7SDN0xhbmdduwbYf1V7i
Osv/m0DXv6T6QHmXkH9Rws3jeDOUXy0RfiNePFDfgnmg2FbZoEuW80poxpioxQ9i
gEPMf0Vyktvxdx7kfQwHvbRvhiRGxs543tdLaKBJtRhjQ3w72zxzYrastlRgDuLo
QUfETsOjCj7ZbV9ZZbkwtb3YgTpSXqRM0QH39lMBGdysa9s/vB7vT1itMgltMxon
Y3bNpN/1Mn0yQXTWcNvrW8h/oExZqTXwkSFrckhRX8W6HvLI2xkL+73gTbWtXYFj
YIHbPkHleU30SmJ4y2/wIzO7D1UXCa49fHb3lkwogZmekMukWPBl7CPOABSz9Ydg
BtHkwG/BElQrxxVUTyIGRuFJarMS2+rQE5gd/RpsCOhfTJ6pGVne3GvVe7uyOQnh
wGbU/etIPvufwePuvq8RNCaAQGnHo/cU3BDY6GFQ8Vez8oyRVGvHCGYzlmrJbryW
/6kZo8URVwI3zv9rFqhdlp3kj6uil4f7FsWHs1aNWjczVvdYSvB/gjrXEqpv2Edp
R8GiJecRsYi5R4XencVTcusEpKr9uD+kHnf6SVygob00tAx/7nP5JqQot72gTQMb
eItSnkYQhpSoy/o0FnW6Ep4=
=WHYJ
-----END PGP SIGNATURE-----
