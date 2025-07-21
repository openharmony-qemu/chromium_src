-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: pciutils
Binary: pciutils, libpci-dev, libpci3, pciutils-udeb, libpci3-udeb
Architecture: any
Version: 1:3.10.0-2build1
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Homepage: https://mj.ucw.cz/sw/pciutils/
Standards-Version: 4.6.2
Vcs-Browser: https://git.hadrons.org/cgit/debian/pkgs/pciutils.git
Vcs-Git: https://git.hadrons.org/git/debian/pkgs/pciutils.git
Testsuite: autopkgtest
Testsuite-Triggers: build-essential, pkg-config
Build-Depends: debhelper-compat (= 13), debhelper (>= 13.10), pci.ids (>= 0.0~2019.11.10-2) <!noudeb>, pkgconf | pkg-config, libkmod-dev [linux-any], libudev-dev (>= 196) [linux-any], zlib1g-dev
Package-List:
 libpci-dev deb libdevel optional arch=any
 libpci3 deb libs optional arch=any
 libpci3-udeb udeb debian-installer optional arch=any profile=!noudeb
 pciutils deb admin standard arch=any
 pciutils-udeb udeb debian-installer optional arch=any profile=!noudeb
Checksums-Sha1:
 1d803c1b3f7a8ca8f34dc62b85e73c6b9100450e 931711 pciutils_3.10.0.orig.tar.gz
 ab8e85a5555d1176178d18d7f95fba1e846bbe51 228 pciutils_3.10.0.orig.tar.gz.asc
 0621648919c2ca0d2c0a54138615874a107c68ef 19664 pciutils_3.10.0-2build1.debian.tar.xz
Checksums-Sha256:
 7deabe38ae5fa88a96a8c4947975cf31c591506db546e9665a10dddbf350ead0 931711 pciutils_3.10.0.orig.tar.gz
 5790fe2b480acc071818e4138aedddc9fd8c1221f4f5077ede7e1237e919c8fb 228 pciutils_3.10.0.orig.tar.gz.asc
 0d81f2e742e6e5d3093c4baa11e3defcdbed1f5d2efba849b871b4f28d658381 19664 pciutils_3.10.0-2build1.debian.tar.xz
Files:
 ca53b87d2a94cdbbba6e09aca90924bd 931711 pciutils_3.10.0.orig.tar.gz
 090515d72c841f6805f3139994bcb706 228 pciutils_3.10.0.orig.tar.gz.asc
 0623f0a70cfacd3bf6bd103c78c4aa49 19664 pciutils_3.10.0-2build1.debian.tar.xz
Original-Maintainer: Guillem Jover <guillem@debian.org>

-----BEGIN PGP SIGNATURE-----

iQJHBAEBCgAxFiEET7WIqEwt3nmnTHeHb6RY3R2wP3EFAmYUGEwTHGp1bGlhbmtA
dWJ1bnR1LmNvbQAKCRBvpFjdHbA/cdlJD/0f8RLHPsP7kYRARxZf/aom2c/IyR8f
zaSSPdpPd/FGak2yNCOuXsKw9izohnlP8/YEuezNYoeicTD0vRC1iPgEHEkb3rgf
9dGQgtV5JCOAWsStyAn91glEGDPxC/v+g/NLjEJDYf7H7fJA4uZvfby4aojcptTl
LO73ovnmLhNamEBCqRjUQlIPVKjQl0BKe3WmN/b6O7sNVmN2s0BA/eYXqgtKYvv0
pPWY9E1AZiY9zpQ4a5ujq8/vN2cZjIs3sFxy1yOxRlCwgdW0UmkXZ5eXN4mZLJ2I
Ym09kdjPNk7uYU7AZXENfu+S005mto7SL1iSMSQgx+ZU+jLCn8nGzpaum5wv1q5U
gzEdtBAuf6uRxA6PwDH47QbSzaem8R1nmTQUk3SkvX7WR4JLh+2d3n4CsWuHOscq
HNByS5OQ4Gx+Cod3k50oIh1PYIvns8igYKGHYaVA1LvPt17QWq76+J+Tel28QQ/C
0Sj5NQ9ezOsaoozOKMwLjiSWnbPO6d6V9m8bdyP/Z8+awNeOESzGysu5sft3izX0
RYeE6ubF02rJJL/hyJzNOkjbCyb9mP8TKkuGW/qUII7th6uEcx0xRnatHtxZY9yW
4MOI9+5bU2WlEl+DV2AwvtJ/pRTV5QZ4qaTYgvoKYElZNncm1JIuzzjrZFhuu7C8
QqBkEHrJewdYYw==
=+Gyu
-----END PGP SIGNATURE-----
