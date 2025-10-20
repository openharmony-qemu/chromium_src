-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: openldap
Binary: slapd, slapd-contrib, slapd-smbk5pwd, ldap-utils, libldap2, libldap-common, libldap-dev, libldap2-dev, slapi-dev
Architecture: any all
Version: 2.6.7+dfsg-1~exp1ubuntu8
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Steve Langasek <vorlon@debian.org>, Torsten Landschoff <torsten@debian.org>, Ryan Tandy <ryan@nardis.ca>, Sergio Durigan Junior <sergiodj@debian.org>
Homepage: https://www.openldap.org/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/openldap-team/openldap
Vcs-Git: https://salsa.debian.org/openldap-team/openldap.git
Testsuite: autopkgtest
Testsuite-Triggers: heimdal-kdc, openssl, samba, schema2ldif, ssl-cert
Build-Depends: debhelper-compat (= 13), dh-apparmor, dpkg-dev (>= 1.17.14), groff-base, heimdal-multidev (>= 7.4.0.dfsg.1-1~) <!pkg.openldap.noslapd>, libargon2-dev <!pkg.openldap.noslapd>, libgnutls28-dev, libltdl-dev <!pkg.openldap.noslapd>, libperl-dev (>= 5.8.0) <!pkg.openldap.noslapd>, libsasl2-dev, libwrap0-dev <!pkg.openldap.noslapd>, nettle-dev <!pkg.openldap.noslapd>, openssl <!nocheck>, perl:any, pkg-config (>= 0.29), po-debconf, unixodbc-dev <!pkg.openldap.noslapd>, krb5-admin-server <!nocheck>, krb5-user <!nocheck>, krb5-kdc <!nocheck>, libsasl2-modules-gssapi-mit <!nocheck>, sasl2-bin <!nocheck>
Build-Conflicts: autoconf2.13, bind-dev, libbind-dev
Package-List:
 ldap-utils deb net optional arch=any
 libldap-common deb libs optional arch=all
 libldap-dev deb libdevel optional arch=any
 libldap2 deb libs optional arch=any
 libldap2-dev deb oldlibs optional arch=all
 slapd deb net optional arch=any profile=!pkg.openldap.noslapd
 slapd-contrib deb net optional arch=any profile=!pkg.openldap.noslapd
 slapd-smbk5pwd deb oldlibs optional arch=all profile=!pkg.openldap.noslapd
 slapi-dev deb libdevel optional arch=any profile=!pkg.openldap.noslapd
Checksums-Sha1:
 80f065b81f691416ea376e10d25ecab3b9e39fb1 3774648 openldap_2.6.7+dfsg.orig.tar.xz
 e11f32f42bcf321cb669d6d1a54c79b53eeba24b 185576 openldap_2.6.7+dfsg-1~exp1ubuntu8.debian.tar.xz
Checksums-Sha256:
 ac9e26700ff8dd10181944d3e61014df5e3db601dd05655966be46757710d6df 3774648 openldap_2.6.7+dfsg.orig.tar.xz
 04a8642ac74086e305334205d84011b1fa143393a432bae64267c40b89c5e8a2 185576 openldap_2.6.7+dfsg-1~exp1ubuntu8.debian.tar.xz
Files:
 36c0542618d106ebef1c15e58b2d80ca 3774648 openldap_2.6.7+dfsg.orig.tar.xz
 834cb50deaaf97b530bea93ac8406170 185576 openldap_2.6.7+dfsg-1~exp1ubuntu8.debian.tar.xz
Original-Maintainer: Debian OpenLDAP Maintainers <pkg-openldap-devel@lists.alioth.debian.org>

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCgAdFiEEVovyKmYzfL/Jprm3LIPbyOm9DjcFAmYNpRcACgkQLIPbyOm9
Djd+Tw//b3NL9mO3kPW2OhSBZCiqXkZ44MM7AB6F77Vsh0XQc1chILQKJ4zGUoz/
lDyrrq6kuSgIBQ676dXtZhcrLLgfGGK1tBYgeFgks7YFBXnLeXk2Yaz4nxs7nxl8
XluGj6RVDLIrfnEOIX385l/u8C5CUGHuyembT6UAdXYjGKXPPmcsULvBjS7OCX9s
qH4HAkXhy46MWjmWGgYTBn1+XgmDxDOakaPwKOrbMwal1hIiB3WNBmMFWVb8oevv
3s4XYXMTDQDEHS/Ouc7+Q7qrSfGA4fNv9sW0mKe/3oK7N3DFEZq5jO5oktMfFNcj
jbkABM+OlLc2Fzshdj9sDk8K2qtFvfEmostR+ZM9NIqV3/AicPB0XI0Noq6bOTG8
x9+IGwI62KxNCtWvakqijRsKp3cyVI4JbYmoXZDrTxell0JkV6vPH1fIVdH5ZuTP
ZojizRLD6SJ7bClWC42H+IctfwG4tzXCVR6hsmcHYC5mmakkHJ+ZUVLMzADOoCyp
qUZ9ywc6OeYQovN9z7FTo9SttDbyBbxPOACjwWJAS2MFXXwkViW4vgGXAQt2JL4T
n72SQ37JNb5GSWfJCsUtg260TdzIy1KkUUdnUDNal2s2jBtnsIeSW27hiccJt28N
9NaNXTUzBDvf4RLCTg4wd8pFjvpRSgkrQDI51TOojke+os0Mp2g=
=maIU
-----END PGP SIGNATURE-----
