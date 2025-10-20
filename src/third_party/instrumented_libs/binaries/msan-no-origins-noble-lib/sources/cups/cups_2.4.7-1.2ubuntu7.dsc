-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA1

Format: 3.0 (quilt)
Source: cups
Binary: libcups2t64, cups, cups-bsd, cups-client, cups-common, cups-core-drivers, cups-daemon, cups-ipp-utils, cups-ppdc, cups-server-common, libcups2-dev, libcupsimage2t64, libcupsimage2-dev
Architecture: any all
Version: 2.4.7-1.2ubuntu7
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders:  Till Kamppeter <till.kamppeter@gmail.com>, Thorsten Alteholz <debian@alteholz.de>,
Homepage: https://github.com/OpenPrinting/cups/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/printing-team/cups
Vcs-Git: https://salsa.debian.org/printing-team/cups.git
Testsuite: autopkgtest
Build-Depends: dpkg-dev (>= 1.22.5), autoconf, automake, debhelper-compat (= 13), dh-strip-nondeterminism, libavahi-client-dev, libavahi-common-dev, libdbus-1-dev, libgnutls28-dev, libkrb5-dev, libpam0g-dev, libpaper-dev, libsystemd-dev [linux-any], libtool, libusb-1.0-0-dev [!hurd-any], patch, pkg-config, po-debconf, po4a, zlib1g-dev, libapparmor-dev, libsnapd-glib-dev (>= 1.61)
Build-Depends-Arch: dpkg-dev (>= 1.22.5), dh-apparmor
Build-Conflicts: libgmp-dev (<< 2:6)
Package-List:
 cups deb net optional arch=any
 cups-bsd deb net optional arch=any
 cups-client deb net optional arch=any
 cups-common deb net optional arch=all
 cups-core-drivers deb net optional arch=any
 cups-daemon deb net optional arch=any
 cups-ipp-utils deb net optional arch=any
 cups-ppdc deb utils optional arch=any
 cups-server-common deb net optional arch=all
 libcups2-dev deb libdevel optional arch=any
 libcups2t64 deb libs optional arch=any
 libcupsimage2-dev deb libdevel optional arch=any
 libcupsimage2t64 deb libs optional arch=any
Checksums-Sha1:
 9c6155dfa367eee9a88ad08cf83b1dc6c446309f 8134809 cups_2.4.7.orig.tar.gz
 195ddf66c05554392d8465bfdec44da091d017e1 402816 cups_2.4.7-1.2ubuntu7.debian.tar.xz
Checksums-Sha256:
 dd54228dd903526428ce7e37961afaed230ad310788141da75cebaa08362cf6c 8134809 cups_2.4.7.orig.tar.gz
 15355cec74f36b5a6165cb962638412159f108169381356653bbaa5ea5302b5f 402816 cups_2.4.7-1.2ubuntu7.debian.tar.xz
Files:
 e0a5ddbf53dfad41da26fc1ef60b2256 8134809 cups_2.4.7.orig.tar.gz
 6789e4bc4140a2653862403aa5828c94 402816 cups_2.4.7-1.2ubuntu7.debian.tar.xz
Original-Maintainer: Debian Printing Team <debian-printing@lists.debian.org>

-----BEGIN PGP SIGNATURE-----

iF0EARECAB0WIQQLOAVJl6EEH3JscYZO5Vq2X9woSQUCZhmhjQAKCRBO5Vq2X9wo
SfXyAKDO163QWF+LdvBf+I3kf/BbKFjEDwCgtfoMi6+2II4ovUz+/N8ov9W0TVU=
=VX16
-----END PGP SIGNATURE-----
