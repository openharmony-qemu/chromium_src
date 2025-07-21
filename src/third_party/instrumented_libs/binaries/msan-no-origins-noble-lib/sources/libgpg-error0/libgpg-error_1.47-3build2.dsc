-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: libgpg-error
Binary: libgpg-error-dev, libgpg-error0, gpgrt-tools, libgpg-error0-udeb, libgpg-error-mingw-w64-dev, libgpg-error-l10n
Architecture: any all
Version: 1.47-3build2
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders:  Daniel Kahn Gillmor <dkg@fifthhorseman.net>, Christoph Biedl <debian.axhn@manchmal.in-ulm.de>,
Homepage: https://www.gnupg.org/related_software/libgpg-error/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/debian/libgpg-error
Vcs-Git: https://salsa.debian.org/debian/libgpg-error.git -b debian/unstable
Testsuite: autopkgtest
Testsuite-Triggers: build-essential, gcc-mingw-w64-i686, gcc-mingw-w64-x86-64, pkg-config, wine32, wine64
Build-Depends: automake (>= 1.14), debhelper-compat (= 13), gettext (>= 0.19.3), texinfo
Build-Depends-Indep: mingw-w64
Package-List:
 gpgrt-tools deb devel optional arch=any
 libgpg-error-dev deb libdevel optional arch=any
 libgpg-error-l10n deb localization optional arch=all
 libgpg-error-mingw-w64-dev deb libdevel optional arch=all
 libgpg-error0 deb libs optional arch=any
 libgpg-error0-udeb udeb debian-installer optional arch=any profile=!noudeb
Checksums-Sha1:
 94668233fd7bd8e7c0ec5e363134cd53c575da60 1020862 libgpg-error_1.47.orig.tar.bz2
 8d7a5385afb1d1c64c99fc566d01009f97c16595 228 libgpg-error_1.47.orig.tar.bz2.asc
 049705342b650a71b71246a3cd6fc163c213744d 18712 libgpg-error_1.47-3build2.debian.tar.xz
Checksums-Sha256:
 9e3c670966b96ecc746c28c2c419541e3bcb787d1a73930f5e5f5e1bcbbb9bdb 1020862 libgpg-error_1.47.orig.tar.bz2
 6ab547bf020761e1df80b08335773a91c345ff2c1344f15b1f7d195293ab21a5 228 libgpg-error_1.47.orig.tar.bz2.asc
 18342531fa931eda1ca0a8b60213ad87fc506f9a7170c1bec07d65ee61c0d320 18712 libgpg-error_1.47-3build2.debian.tar.xz
Files:
 58e054ca192a77226c4822bbee1b7fdb 1020862 libgpg-error_1.47.orig.tar.bz2
 4a8886f8fd54bd4969dccf994f3909ca 228 libgpg-error_1.47.orig.tar.bz2.asc
 7fe84d3c49b2b8ebe4353fc30ee75830 18712 libgpg-error_1.47-3build2.debian.tar.xz
Original-Maintainer: Debian GnuPG Maintainers <pkg-gnupg-maint@lists.alioth.debian.org>

-----BEGIN PGP SIGNATURE-----

iQJHBAEBCgAxFiEET7WIqEwt3nmnTHeHb6RY3R2wP3EFAmYUFagTHGp1bGlhbmtA
dWJ1bnR1LmNvbQAKCRBvpFjdHbA/caNxEACeLLLzfEiddNOhQgifzjaMkxUUnVAA
sxpP8jrjytXL86PYzFdO6DF7X/M6o429Oqbwnm4VRXreuJkaCuXIxfeEZMymt4cz
1ifB56KAT3CuEDobvTIcwANv+dscS/tFqL6ayd9bw1am+PDuR19S2+nA+5+pRdK3
ZKSPlbZ8+qKTS3tI6b7k32f0YpDrhP2d+h3JwZ8UQw8z5r8Kq6UcRuyz9Rz8wd82
GXrWqnlcVDsWIgSwapA9ZSaItTqhYM6M5JHDDYh6i1m45ghnHU8e9h/VRrdWgMGj
SfXn/ndySNRHpTF+JF/7dAsE+g5LR+Ofom8t+fFntabUjhhfIUI4U3Amyn9ibQmj
xub7YUM0SNHfj1bVcSydWHhxRDD8BEsWc48Uc4f6gQXrsftZB6GadfhOLV+VOX20
Nv2tmmZtsIhzpIfcWxZ/ntnxFXJMvtorw9K/MKmN1ak2vn5Xhi74x91grF9czKqZ
OODwybRmYKHPFgJ71j61WD8V1BGkeY0J37xKY5TQ2V63a2a4I2naBThZft8v8UfD
0Mn2BZWaSuq/hzzEJnGfdnK0pUdaIyyV7lWeMY4mbv9QtWx9pq1rOBJYakGCT5N8
X4FkiQnAm5xDzLzoGsf1VFRBC5uex9ynWZuD9bpX7fLalJ/h1jKr7tKQB1p4gQds
2NacLahze1GQbA==
=2fZe
-----END PGP SIGNATURE-----
