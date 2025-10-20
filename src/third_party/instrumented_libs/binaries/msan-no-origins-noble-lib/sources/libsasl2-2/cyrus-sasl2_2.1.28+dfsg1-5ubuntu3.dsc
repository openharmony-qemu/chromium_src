-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: cyrus-sasl2
Binary: sasl2-bin, cyrus-sasl2-doc, libsasl2-2, libsasl2-modules, libsasl2-modules-db, libsasl2-modules-ldap, libsasl2-modules-otp, libsasl2-modules-sql, libsasl2-modules-gssapi-mit, libsasl2-dev, libsasl2-modules-gssapi-heimdal
Architecture: any all
Version: 2.1.28+dfsg1-5ubuntu3
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Ondřej Surý <ondrej@debian.org>,
Homepage: https://www.cyrusimap.org/sasl/
Standards-Version: 4.3.0.1
Vcs-Browser: https://salsa.debian.org/debian/cyrus-sasl2
Vcs-Git: https://salsa.debian.org/debian/cyrus-sasl2.git
Testsuite: autopkgtest
Testsuite-Triggers: krb5-admin-server, krb5-kdc, krb5-user, ldap-utils, slapd
Build-Depends: chrpath, debhelper-compat (= 12), default-libmysqlclient-dev <!pkg.cyrus-sasl2.nosql>, docbook-to-man, groff-base, heimdal-multidev <!pkg.cyrus-sasl2.nogssapi>, krb5-multidev <!pkg.cyrus-sasl2.nogssapi>, libdb-dev, libcrypt-dev, libkrb5-dev <!pkg.cyrus-sasl2.nogssapi>, libldap2-dev <!pkg.cyrus-sasl2.noldap>, libpam0g-dev, libpod-pom-view-restructured-perl:native, libpq-dev <!pkg.cyrus-sasl2.nosql>, libsqlite3-dev, libssl-dev, po-debconf, python3-six, python3-sphinx-rtd-theme:native
Build-Conflicts: heimdal-dev
Package-List:
 cyrus-sasl2-doc deb doc optional arch=all
 libsasl2-2 deb libs optional arch=any
 libsasl2-dev deb libdevel optional arch=any
 libsasl2-modules deb libs optional arch=any
 libsasl2-modules-db deb libs optional arch=any
 libsasl2-modules-gssapi-heimdal deb libs optional arch=any profile=!pkg.cyrus-sasl2.nogssapi
 libsasl2-modules-gssapi-mit deb libs optional arch=any profile=!pkg.cyrus-sasl2.nogssapi
 libsasl2-modules-ldap deb libs optional arch=any profile=!pkg.cyrus-sasl2.noldap
 libsasl2-modules-otp deb libs optional arch=any
 libsasl2-modules-sql deb libs optional arch=any profile=!pkg.cyrus-sasl2.nosql
 sasl2-bin deb utils optional arch=any
Checksums-Sha1:
 297ba17d605b880a9a9beb8b08abc0cf90945c40 794540 cyrus-sasl2_2.1.28+dfsg1.orig.tar.xz
 c8d2bc0034b14a0d6ce5b8ea45deab61446ea214 98208 cyrus-sasl2_2.1.28+dfsg1-5ubuntu3.debian.tar.xz
Checksums-Sha256:
 e796a5d85d1a85e1b433d43504e467f9075c7ebc0b45730a3996cf11b1deada4 794540 cyrus-sasl2_2.1.28+dfsg1.orig.tar.xz
 455d738149b4c718ff5b448642ec241e67d61196f7a5228d0cd4c9d172af6bab 98208 cyrus-sasl2_2.1.28+dfsg1-5ubuntu3.debian.tar.xz
Files:
 3ab6f193b23938524b51706616d38ad1 794540 cyrus-sasl2_2.1.28+dfsg1.orig.tar.xz
 025a5884039b468986f3866cad4c1f94 98208 cyrus-sasl2_2.1.28+dfsg1-5ubuntu3.debian.tar.xz
Original-Maintainer: Debian Cyrus Team <team+cyrus@tracker.debian.org>

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCgAdFiEE6S/Qs2sU8fTY4OsvEG2hyMPLvxQFAmYQWG8ACgkQEG2hyMPL
vxTtJw/+PHoqI8fw1GqCztcdjiwn0XFJU3m31ana7YjK5+W3JPAIZyY3ENbZJ3EZ
/PDlzDL7Wtncry2dKZqH/fz862sAm7+tmYFYER70G7IMKvLLuUY9YuVjUE6Utn5Y
zN8qTf/TbQexfkiLq4fK5ckDTRxGVstXW/ec5dhecN/R6wgy23QGrOyCfMlFMCdR
xFxR5zcnq2FX8BqbBDUeKzu6wR3sTqBP09R1MJNL5v+xvD/P4tDB/69IF2AyyJuy
VVSsP6YNtG3JrjrZtfWVUOEcbzDG5ECUS5RqO/2043aI3dt7PD2/pYM3OqIL/pan
YtyeAW0vIsHX0REDF9jMkrF4u+3YiGZUEDduaQKS02v0WrbGl32fdGDuuw7upATC
lpRquCiwAL7fA2FRACm2dKBVlGHajqowTqVXKUg+PWomgaVVMNYvV/XFt9VXGryK
pYkZ5OdN/babivR8a/rV2knMNkWqIDseufeoJ7VLlnol7gURhoOc7bOYP8UtyLbK
MSBYchAWwGiWKiuJNQjgZu080Q+3mX6ORU5b9Iw0D1SlXFqCtjcDixM4d/nFwy5i
kud0FllMvUjHTOYKVyPFqs8kO79Z+Tkftl2AdY1iS/o/d4I3Oz6EbNMWVrPK7FM1
qJdUaTSDJ66WqnNKUgrVjfc5EmtF7hWH8eMdH9Ni5J7G3ZdvEHA=
=FhuO
-----END PGP SIGNATURE-----
