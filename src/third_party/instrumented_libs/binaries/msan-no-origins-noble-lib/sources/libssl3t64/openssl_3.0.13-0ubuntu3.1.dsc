-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: openssl
Binary: openssl, libssl3t64, libcrypto3-udeb, libssl3-udeb, libssl-dev, libssl-doc
Architecture: any all
Version: 3.0.13-0ubuntu3.1
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Christoph Martin <christoph.martin@uni-mainz.de>, Kurt Roeckx <kurt@roeckx.be>, Sebastian Andrzej Siewior <sebastian@breakpoint.cc>
Homepage: https://www.openssl.org/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/debian/openssl
Vcs-Git: https://salsa.debian.org/debian/openssl.git
Testsuite: autopkgtest
Testsuite-Triggers: perl
Build-Depends: dpkg-dev (>= 1.22.5), debhelper-compat (= 13)
Package-List:
 libcrypto3-udeb udeb debian-installer optional arch=any profile=!noudeb
 libssl-dev deb libdevel optional arch=any
 libssl-doc deb doc optional arch=all
 libssl3-udeb udeb debian-installer optional arch=any profile=!noudeb
 libssl3t64 deb libs optional arch=any
 openssl deb utils optional arch=any
Checksums-Sha1:
 18b985dcd3fc0bab54cc4bfc10fa9a80ce9e345d 15294843 openssl_3.0.13.orig.tar.gz
 734f6b934c591fbbe4aa93c1a9cf4b5605947121 154740 openssl_3.0.13-0ubuntu3.1.debian.tar.xz
Checksums-Sha256:
 88525753f79d3bec27d2fa7c66aa0b92b3aa9498dafd93d7cfa4b3780cdae313 15294843 openssl_3.0.13.orig.tar.gz
 18849209a25f57ff1da47f7dbfc57864cbbc3cf3030ca35d5e370578bc8cd5f7 154740 openssl_3.0.13-0ubuntu3.1.debian.tar.xz
Files:
 c15e53a62711002901d3515ac8b30b86 15294843 openssl_3.0.13.orig.tar.gz
 7b3b921a3efabf1528de31d10a36cd90 154740 openssl_3.0.13-0ubuntu3.1.debian.tar.xz
Original-Maintainer: Debian OpenSSL Team <pkg-openssl-devel@alioth-lists.debian.net>

-----BEGIN PGP SIGNATURE-----

iQJaBAEBCgBEFiEE4I1aChuaH07AdH+adlTpHiRAJW0FAmZDaVEmHGRhdmlkLmZl
cm5hbmRlemdvbnphbGV6QGNhbm9uaWNhbC5jb20ACgkQdlTpHiRAJW05cRAAqN9H
uZqBt1rwik5GPy7wdWOsBTzFnf5uIb+JeAWn23qKihpGkjwZpzS+61pbj8wDwyL0
nSMWmKm4ulOUGdSKBL1/HE3mM/RRiutBUeouJQFKWs100cIU/sK6gX1EroJzpK/m
4vk5D2gkloOCbk6APOSM2NfbCOPZn0Auy+7txFktN8Kv8rOg/LvIabNP/L6P8oqs
49mgU/c60Hm+VULIZUWFGpx8cKzjcNe8aQDDdhRmWkApOZ1S9220oDVuAYU0nKrm
asoni2WMqkJHOUDOB7u0P429x0PVDC6xWtEiz5EE57oxZe6rWQB79d7os7j3L2N6
v1EyoCQPKlGUZTEAuwusw+ELX5zxI4pGBB24DhCJHa+oNkyBEZstNlgtOHmA2xmL
T0yHz9M/8YmZwPWpZ+4WsavpaXt+fyZt0bSEJ9VPHrfJu+0S+ZmywK7N8vVzbAiX
Rq6GMyJLM0d5AL7KukzclQt10r1FY/XS7A1TkIIcM5tDRQUtSXRYrZt0U1e+vap1
fcLV79i18HzZgmNW7+/0OTTfX8gTcnAqhSA9ibfwBUJJxjlL6t8I1DdOuDbJVf0y
8u5RkIn2dfeEULdeHCeFSwh1GyMvjHV/UbHvkSKq+ET9DgLteQ7zUmdx0i+/Y0a8
wkla9go58pCR34M4v0AujCOTO8dRfXsBvFjUKuY=
=j9Ms
-----END PGP SIGNATURE-----
