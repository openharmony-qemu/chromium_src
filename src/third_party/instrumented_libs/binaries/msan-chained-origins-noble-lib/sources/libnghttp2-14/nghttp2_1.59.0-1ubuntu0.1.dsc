-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: nghttp2
Binary: libnghttp2-dev, libnghttp2-doc, libnghttp2-14, nghttp2-client, nghttp2-proxy, nghttp2-server, nghttp2
Architecture: any all
Version: 1.59.0-1ubuntu0.1
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Ondřej Surý <ondrej@debian.org>
Homepage: https://nghttp2.org/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/debian/nghttp2
Vcs-Git: https://salsa.debian.org/debian/nghttp2.git
Testsuite: autopkgtest
Testsuite-Triggers: nginx
Build-Depends: debhelper (>= 13), debhelper-compat (= 13), libc-ares-dev, libcunit1-dev <!nocheck>, libev-dev, libjansson-dev, libjemalloc-dev [!hurd-i386], libssl-dev, libsystemd-dev, libxml2-dev, pkg-config, zlib1g-dev
Build-Depends-Indep: python3-sphinx, python3-sphinx-rtd-theme
Package-List:
 libnghttp2-14 deb libs optional arch=any
 libnghttp2-dev deb libdevel optional arch=any
 libnghttp2-doc deb doc optional arch=all
 nghttp2 deb httpd optional arch=all
 nghttp2-client deb httpd optional arch=any
 nghttp2-proxy deb httpd optional arch=any
 nghttp2-server deb httpd optional arch=any
Checksums-Sha1:
 ea1a278e07b4697f502b66a353ad8171657e6013 1055492 nghttp2_1.59.0.orig.tar.gz
 f7c243acb31db31c12fe361f14590b59bb80e098 14148 nghttp2_1.59.0-1ubuntu0.1.debian.tar.xz
Checksums-Sha256:
 0dd5c980f1262ff5f03676fd99f46f250b66c842f7d864fa1ca9f8453e5f8868 1055492 nghttp2_1.59.0.orig.tar.gz
 87f3dd9aa3187c7e3b88cf80eb53d7244a5d8ac7f792219cb450dd8625e3c800 14148 nghttp2_1.59.0-1ubuntu0.1.debian.tar.xz
Files:
 7768dc83ead00b5833214204c2acfd5e 1055492 nghttp2_1.59.0.orig.tar.gz
 860ed0f5e7fb33e52c8cdaf1db5e3276 14148 nghttp2_1.59.0-1ubuntu0.1.debian.tar.xz
Original-Maintainer: Tomasz Buchert <tomasz@debian.org>

-----BEGIN PGP SIGNATURE-----

iQJRBAEBCgA7FiEE2WgtvmwmcgaEBLlnCAvK1QvD6SAFAmY5AEcdHGZhYmlhbi50
b2VwZmVyQGNhbm9uaWNhbC5jb20ACgkQCAvK1QvD6SCqXA//YH7DXOOs/vhPcYSk
n07uwxC8Ooxj6piYFOrwlfC+qb4uGa0i3imXZ7EPgQGSY97s3+PrvE/u6YykwB30
59s87QGC1faZjWxCfVFnADYuvSL9lCEbe4YTFr/0PLp27WDO8HGoOSHehmNGCIOt
thY6yT6ACWmkqauepv8nI2DUcreTeeiqHJJIK4ACJ7AfIdLV23e1ihqux7YESEF4
8XApvCS+/WCcewCrNNrA8SmUQnCEfyPAwsIanp+X2MuiBIrs9IfxYKQa6aZf3IHC
xh41H2NSUm9buV8Iyx4IDdEO+Nfccxww8nVPNrxx2SALfxXGo3+KvWL1NfcRkqPO
DmqcYUd6GmeTxrE0aDZGb35diZj8bC2n5IDPE/JXhFsA00/DR+i43c0SmxJGmzqs
Ctfl8TEX50x2+VWe4iiPxK6l0FCo56j+OKrIcbvDImu4Etks0YDVlE6+6uW7UMhZ
XkrKE+kakM4q5fg1VUkIMtWP0liRhuEPX34KoDipGR5WrE55rIw1/ck7hEDLw8cz
l309I9UfgAyZexEB0VWyWSO8yaybdPDc3k+3NJhRtvFhknvrexWvpkZCUikHzy2H
PZBMFjxHooPAknoURM4wHFefUqEvScW+24tT4h6VF/1muwzQMZY2PtfvKtAnHbDx
lf/++vq2WLi3jLegzP5/MvrM/x0=
=ceMt
-----END PGP SIGNATURE-----
