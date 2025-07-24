-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: gnutls28
Binary: libgnutls28-dev, libgnutls30t64, gnutls-bin, gnutls-doc, libgnutls-openssl27t64, libgnutls-dane0t64
Architecture: any all
Version: 3.8.3-1.1ubuntu3.1
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders:  Andreas Metzler <ametzler@debian.org>, Eric Dorland <eric@debian.org>, James Westby <jw+debian@jameswestby.net>, Simon Josefsson <simon@josefsson.org>,
Homepage: https://www.gnutls.org/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/gnutls-team/gnutls
Vcs-Git: https://salsa.debian.org/gnutls-team/gnutls.git
Testsuite: autopkgtest
Testsuite-Triggers: build-essential, ca-certificates, datefudge, freebsd-net-tools, net-tools, openssl, softhsm2
Build-Depends: dpkg-dev (>= 1.22.5), bison, ca-certificates <!nocheck>, chrpath, datefudge <!nocheck>, debhelper-compat (= 13), freebsd-net-tools [kfreebsd-i386 kfreebsd-amd64] <!nocheck>, gperf, libcmocka-dev <!nocheck>, libidn2-dev, libp11-kit-dev, libssl-dev <!nocheck>, libtasn1-6-dev, libunbound-dev, libunistring-dev, net-tools [!kfreebsd-i386 !kfreebsd-amd64] <!nocheck>, nettle-dev (>= 3.6), openssl <!nocheck>, pkg-config, python3:any, softhsm2 <!nocheck>
Build-Depends-Indep: gtk-doc-tools, texinfo, texlive-latex-base, texlive-plain-generic
Build-Conflicts: libgnutls-dev
Package-List:
 gnutls-bin deb net optional arch=any
 gnutls-doc deb doc optional arch=all
 libgnutls-dane0t64 deb libs optional arch=any
 libgnutls-openssl27t64 deb libs optional arch=any
 libgnutls28-dev deb libdevel optional arch=any
 libgnutls30t64 deb libs optional arch=any
Checksums-Sha1:
 806156ac9563caab642d6274496b9cc5b2117612 6463720 gnutls28_3.8.3.orig.tar.xz
 e256237d54d45e4b6f46fe692651d6dea33f359c 854 gnutls28_3.8.3.orig.tar.xz.asc
 e24fbb9d58052817c0c425e84b16bfd6be0a420e 88208 gnutls28_3.8.3-1.1ubuntu3.1.debian.tar.xz
Checksums-Sha256:
 f74fc5954b27d4ec6dfbb11dea987888b5b124289a3703afcada0ee520f4173e 6463720 gnutls28_3.8.3.orig.tar.xz
 b2b90d225728890b0e2aa7c05e5f25f8ba1282821b46e72cd99f0c732b639cef 854 gnutls28_3.8.3.orig.tar.xz.asc
 fea67ee2c58708cd85f9b62a7dc15a4729bfcbb2b73d6f950203047f9184d328 88208 gnutls28_3.8.3-1.1ubuntu3.1.debian.tar.xz
Files:
 269966167fa5bf8bae5f7534bcc3c454 6463720 gnutls28_3.8.3.orig.tar.xz
 8da0da357d6762b98a491bf0a3fc6e1f 854 gnutls28_3.8.3.orig.tar.xz.asc
 bce523afef50752007ee418faffe4abd 88208 gnutls28_3.8.3-1.1ubuntu3.1.debian.tar.xz
Original-Maintainer: Debian GnuTLS Maintainers <pkg-gnutls-maint@lists.alioth.debian.org>

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCgAdFiEEUMSg3c8x5FLOsZtRZWnYVadEvpMFAmYmnjoACgkQZWnYVadE
vpMfJw/9FfQvVF4ig7c3QkjWrD1G8IpPgXBcNsbc2AA/E5uVsDRTCQqJmwScdIWb
j1rdrytWkiyTs9ks9OFghX1E0J58ZMcG8V63aqNgpSuIdtdY+UDCdZXBYlJvRXU3
xFLgF8WcPZFnUp9RriZ8aORxDYoR/4d23abcUoPZ+kXXU18eXITvhSplvS0mEs0D
ItzvaMiKCbz5G0wvLZ2iEYKd9CCJ2Kky10+KO+Zu62BZ407xl9n4B5AKns72kaTS
Hq+GwO9uZPBSwwzxFOhcmq6podOBGMZDEU1wqkRezCe2+vSaBRTYtVw6pHneM0MG
uFQVNKBZI3o3ZO/yLDMM8ft1P48lCmRazR0bCh8MKuEeaI/1ZMibdumRlQaDxxTO
+ygwt8CQGQxHZ1pCZc7tfwWk5/9+Jee6bVicl08v73EaecNYoePTCyyBsrI/McV5
86NDLM+HnAsPJul7dgvCXWkbJ8goZCdhzB3Us7iU3dDzQ/2su0Lt7yYNFq8A97xs
Hx0rgBFyt+d45lOr2a6gCz2U3FXqlIZ9yWa7s5szn0f1Ywgg2Qf8dSGlL7wZHWuG
F1nTtMmyai3VwOlavDr2JBgVdVcXfCNbwgOjslu1Chvnab/K4Jcg1vZRqLKf6sgl
4z6h7zG/8qjyxUQoY9km6awiBG7qsCvwFF153cs970rxCrA9FiA=
=bthg
-----END PGP SIGNATURE-----
