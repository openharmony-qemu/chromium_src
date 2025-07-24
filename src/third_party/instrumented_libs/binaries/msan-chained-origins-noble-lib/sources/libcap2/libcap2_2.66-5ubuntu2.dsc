-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: libcap2
Binary: libcap2-bin, libcap2, libcap-dev, libpam-cap, libcap2-udeb
Architecture: linux-any
Version: 1:2.66-5ubuntu2
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Homepage: https://sites.google.com/site/fullycapable/
Standards-Version: 4.6.1
Vcs-Browser: https://salsa.debian.org/debian/libcap2
Vcs-Git: https://salsa.debian.org/debian/libcap2.git
Testsuite: autopkgtest
Testsuite-Triggers: gcc, libc-dev
Build-Depends: chrpath, debhelper-compat (= 13), gperf (>= 3.1), indent, libpam0g-dev
Package-List:
 libcap-dev deb libdevel optional arch=linux-any
 libcap2 deb libs optional arch=linux-any
 libcap2-bin deb utils important arch=linux-any
 libcap2-udeb udeb debian-installer optional arch=linux-any profile=!noudeb
 libpam-cap deb admin optional arch=linux-any
Checksums-Sha1:
 d440e60686bd542e2a6481986c63ee8de625563e 181592 libcap2_2.66.orig.tar.xz
 361fc8e7841f3b6e680f21f2475365021a98b2b6 22212 libcap2_2.66-5ubuntu2.debian.tar.xz
Checksums-Sha256:
 15c40ededb3003d70a283fe587a36b7d19c8b3b554e33f86129c059a4bb466b2 181592 libcap2_2.66.orig.tar.xz
 13d7a7f6dc6776096ca9351d2e1841f46b6f1fb34791c8e9ac8a5403061ffa88 22212 libcap2_2.66-5ubuntu2.debian.tar.xz
Files:
 00afd6e13bc94b2543b1a70770bdb41f 181592 libcap2_2.66.orig.tar.xz
 65dd3dd644aed60fa0616d556bbda473 22212 libcap2_2.66-5ubuntu2.debian.tar.xz
Original-Maintainer: Christian Kastner <ckk@debian.org>

-----BEGIN PGP SIGNATURE-----

iQJHBAEBCgAxFiEET7WIqEwt3nmnTHeHb6RY3R2wP3EFAmYUFSoTHGp1bGlhbmtA
dWJ1bnR1LmNvbQAKCRBvpFjdHbA/cdnlD/0apkf3UDUdq0n/7KNfYJePm4BaAmKC
jdFUNU9Xeqv1f17hH1XsFhorbqJwcSRFKnkRybVKkQNG4+gqQph7326fDyPvBcIt
lvfYztlhNxSJj2xloq2MTOqSGI76JVfVbTxliUIthtcOXBzy5O7uxWAdR2bbNgXY
f2ToUk0chsadk8pzTNeAGSYJnwAy9C77FTplD6A9ApfEiXRPs9DJShVVBHyYKNnV
i1VgB4zxy3KzHbzYrKy9ydl9tkWr1NrK96djbO9GHuDgcGPtEUUhrFEeXOSSxqKj
M0EOrIoDUXUHeje3QOETbvsWtmQpebLJ6pGwSQlBc8FK2LF1ImPKiUrknpIEYVI6
Fz/TKj5SXDzUFeyj3gkkwzJsIRAgWR11iRnDZY6XjF1qzD1G9ok82OtUyKLFgfzd
74qL6Aws8eUgmwC2RUa8FTYKYw8k26AMwc5tW8H+ewhFcCRGCRSwVCtZ7McLlfPW
ZRIX10qSUInHCgGkCHd+3KAYil1h53osLjPmYRBbJ2QnKK/YpVImplqKK1TzdfkZ
khmhIC4nWswsEgnAkzargnL5oWAFFyeKF/ouqMroUIqZrYlwXfnZT4a6sLMQB6F9
ixkMH6IMj/8EIFvTUMChxQGJythAvvGefDkSkIBICO5tPoBqWD0ixTBIbdL/4gqQ
9w/FxJElq1sQYQ==
=TRhF
-----END PGP SIGNATURE-----
