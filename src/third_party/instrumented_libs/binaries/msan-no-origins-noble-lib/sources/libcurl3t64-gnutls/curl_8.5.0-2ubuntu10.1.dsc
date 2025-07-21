-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: curl
Binary: curl, libcurl4t64, libcurl3t64-gnutls, libcurl4-openssl-dev, libcurl4-gnutls-dev, libcurl4-doc
Architecture: any all
Version: 8.5.0-2ubuntu10.1
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Carlos Henrique Lima Melara <charlesmelara@riseup.net>, Samuel Henrique <samueloph@debian.org>, Sergio Durigan Junior <sergiodj@debian.org>
Homepage: https://curl.se/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/debian/curl
Vcs-Git: https://salsa.debian.org/debian/curl.git
Testsuite: autopkgtest
Testsuite-Triggers: @builddeps@, gcc, libc-dev, libcurl-dev, libldap-dev, pkgconf, slapd
Build-Depends: dpkg-dev (>= 1.22.5), debhelper-compat (= 13), autoconf, automake, ca-certificates, dh-exec, groff-base, libbrotli-dev, libgnutls28-dev, libidn2-dev, libkrb5-dev, libldap2-dev, libnghttp2-dev, libpsl-dev, librtmp-dev, libssh-dev, libssh2-1-dev, libssl-dev, libtool, libzstd-dev, locales-all <!nocheck>, openssh-server <!nocheck>, python3:native <!nocheck>, python3-impacket [!i386] <!nocheck>, gnutls-bin [amd64 arm64 armel armhf i386 mips64el mipsel s390x powerpc ppc64 riscv64] <!nocheck>, quilt, stunnel4 [!i386] <!nocheck>, zlib1g-dev
Build-Conflicts: autoconf2.13, automake1.4
Package-List:
 curl deb web optional arch=any
 libcurl3t64-gnutls deb libs optional arch=any
 libcurl4-doc deb doc optional arch=all
 libcurl4-gnutls-dev deb libdevel optional arch=any
 libcurl4-openssl-dev deb libdevel optional arch=any
 libcurl4t64 deb libs optional arch=any
Checksums-Sha1:
 d7877b39b6efda66e9ac5f9c28b8b28d8c0b7b7e 4372979 curl_8.5.0.orig.tar.gz
 3dae44e5a8b6e35dd653a953ada58f613877630c 53468 curl_8.5.0-2ubuntu10.1.debian.tar.xz
Checksums-Sha256:
 05fc17ff25b793a437a0906e0484b82172a9f4de02be5ed447e0cab8c3475add 4372979 curl_8.5.0.orig.tar.gz
 3a044e477bcd7af48487fb082039b64ea33ae867407bc32e93d0cd2585a1a194 53468 curl_8.5.0-2ubuntu10.1.debian.tar.xz
Files:
 0bc69288b20ae165ff4b7d6d7bbe70d2 4372979 curl_8.5.0.orig.tar.gz
 7fc553b73b3348eef5b18d6f3566f344 53468 curl_8.5.0-2ubuntu10.1.debian.tar.xz
Original-Maintainer: Debian Curl Maintainers <team+curl@tracker.debian.org>

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCgAdFiEEUMSg3c8x5FLOsZtRZWnYVadEvpMFAmYmnBwACgkQZWnYVadE
vpP/WxAAnB5rizI9lfDwp8o+9MPEeH/xd1GMAbaj6L07YBZqYtaDkqAUfMYsInJe
VLPO6SyHCGR9WBaZK81zmlwYqgQHfxxyZtgQHNoXqeRLcV2OQGG3vzk0oxHu9cvd
9iDFwc0ydyFWf/SCP9rruEfVaCxaoqspxRNIqOc9gH1sYfA6Es4VCvVI0L5aQVUE
Y+0p0EIP9+RzkwwNGleqnLu2M89qyTO5MbTx75KGQKUCxNnMUDmx9fko5mTudykg
Sj2wErVf4tKfe9FunE+1WjTYkqrAjc1gQKg2onLRQyQtNIj1DWJjZE7HzZ9Wbo7A
2tYfyju9YXJSAu852czJ5SMljwYtWJvur3mtwKAXTh+e2R8h0V3d/Rrvm2dbUB7N
BnnzDofCcPFLgjMg3+0t4MDrKj1zac8I99+zH9Npid3vy4oXirRorHlUNNf1yOAR
JU/XU9iP8wW++Wckh80WJzA5yc/ijyGg8rretX/+zfZ3r5w+4JYGgHPkdxwfIb6f
LtKr0X0+jqnz0FoCHLj+yLheJDWPrJfUjDXY5RETZEpOm7NZrE52bUQR2+OKD1Ed
INAIZKXUEl65igjc+EDgRtOd/6VEQDpHDdfJCidpTRgZ4KucaGANTUw3yRpHoyUV
S8Zq1GPxIEcrtVfBzEGHmCusaS9N0g9Pqbnh3Y1rxb5wl088SsI=
=59o+
-----END PGP SIGNATURE-----
