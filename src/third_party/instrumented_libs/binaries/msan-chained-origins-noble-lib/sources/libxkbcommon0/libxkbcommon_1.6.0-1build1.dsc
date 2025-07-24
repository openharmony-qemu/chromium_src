-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: libxkbcommon
Binary: libxkbcommon0, libxkbcommon-dev, libxkbcommon-doc, libxkbcommon-tools, libxkbcommon-x11-0, libxkbcommon-x11-dev, libxkbregistry0, libxkbregistry-dev
Architecture: any all
Version: 1.6.0-1build1
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Michael Stapelberg <stapelberg@debian.org>
Homepage: https://www.xkbcommon.org/
Standards-Version: 4.6.1
Vcs-Browser: https://salsa.debian.org/xorg-team/lib/libxkbcommon
Vcs-Git: https://salsa.debian.org/xorg-team/lib/libxkbcommon.git
Testsuite: autopkgtest
Testsuite-Triggers: build-essential, pkg-config
Build-Depends: debhelper-compat (= 13), bison, dh-exec, doxygen, flex, graphviz, meson, pkg-config, quilt, libwayland-dev [linux-any], libxcb-xkb-dev, libxml2-dev, wayland-protocols [linux-any], x11-xkb-utils <!nocheck>, x11proto-dev, xkb-data <!nocheck>, xvfb <!nocheck>
Package-List:
 libxkbcommon-dev deb libdevel optional arch=any
 libxkbcommon-doc deb doc optional arch=all
 libxkbcommon-tools deb graphics optional arch=any
 libxkbcommon-x11-0 deb libs optional arch=any
 libxkbcommon-x11-dev deb libdevel optional arch=any
 libxkbcommon0 deb libs optional arch=any
 libxkbregistry-dev deb libdevel optional arch=any
 libxkbregistry0 deb libs optional arch=any
Checksums-Sha1:
 95a9547dd6efe72fd52b33c5f37a0317e8704bde 510456 libxkbcommon_1.6.0.orig.tar.xz
 99729c41d1d302c266aa3177708a5f680ddc297c 8372 libxkbcommon_1.6.0-1build1.debian.tar.xz
Checksums-Sha256:
 0edc14eccdd391514458bc5f5a4b99863ed2d651e4dd761a90abf4f46ef99c2b 510456 libxkbcommon_1.6.0.orig.tar.xz
 f193f739748e1f6bf28eb27ea0997da93eb20dca3557ae426fefc8f210aa95bb 8372 libxkbcommon_1.6.0-1build1.debian.tar.xz
Files:
 90079ab4a0c6fa56dc75abffef9b1bc6 510456 libxkbcommon_1.6.0.orig.tar.xz
 c30053a3539d32f4bd026c3fa322707a 8372 libxkbcommon_1.6.0-1build1.debian.tar.xz
Original-Maintainer: Debian X Strike Force <debian-x@lists.debian.org>

-----BEGIN PGP SIGNATURE-----

iQJHBAEBCgAxFiEET7WIqEwt3nmnTHeHb6RY3R2wP3EFAmYUFwkTHGp1bGlhbmtA
dWJ1bnR1LmNvbQAKCRBvpFjdHbA/cas2D/48LayiOtlf0Ax/8JHrLXdbeB2P4+cf
WzQuwTHDQqdACtvC1HQpZwV5ed/qjMDUBqtfd4r1YjjU+WJeoSIFqLw01Y7teTST
KIycZy1PtvBpGJzXWUuP+e8dOIm3e/9ZiPTAG+Pju2z8yKI20fF+HikeQZpY2EOc
2kvcLrkOB9A2vb2SyJjNHhSxCb6Do0kUmntPtUADFG/oqK6iLUFHB47YNCyaqEo4
UqMRd2dRPVF3/aui0TG6JPKw2vIdm39XGNgLaYxPSiSRhHPEbicSbpNKoLI97oWv
dPfsRfJQlJqXWMemGc2qhfoiDm2ggzIKhyt+spkLw5MurzQ9xbMe8I0mqZPShrtG
eubvMBBL8x/oamGdB0bnPeJObevY7TOIiZukffmcEwU1r7+zAgvhSUFyG0dVcNVO
ppOPJdZBMIXDdxxSJ7/lHmwkUXky8UZQ38E0k0QWk5KF0U47N7OvuN4wh1EOvMTH
2mEOL1YFdI6ymRjPOKBKas88mU4h3NTozqr9wOZTvLL8XerMVvjvW/F1eDj+HVRs
gJx83XxufBQSD8qa3DQ8OA2+7QzfsFD7G5yxezK3PgXdFwOkXnCFZt8BGIjrowCN
C+5nTu2F+75TKgrmnfsI59uxRNohIoRQfhAH7+m7KdUzZO6OpsootuP4QZgIZG0Q
KdCc7JQU6KNjKg==
=UGtn
-----END PGP SIGNATURE-----
