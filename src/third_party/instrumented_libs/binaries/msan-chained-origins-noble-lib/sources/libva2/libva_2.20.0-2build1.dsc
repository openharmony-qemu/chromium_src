-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: libva
Binary: libva-dev, libva2, libva-x11-2, libva-glx2, libva-drm2, libva-wayland2, va-driver-all
Architecture: any
Version: 2.20.0-2build1
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders:  Reinhard Tartler <siretart@tauware.de>, Sebastian Ramacher <sramacher@debian.org>
Homepage: https://github.com/intel/libva
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/multimedia-team/libva
Vcs-Git: https://salsa.debian.org/multimedia-team/libva.git
Build-Depends: debhelper-compat (= 13), libdrm-dev, libgl-dev, libwayland-dev [linux-any], libx11-dev, libx11-xcb-dev, libxcb-dri3-dev, libxcb1-dev, libxext-dev, libxfixes-dev, meson, ninja-build, perl:any, pkgconf
Package-List:
 libva-dev deb libdevel optional arch=any
 libva-drm2 deb libs optional arch=any
 libva-glx2 deb libs optional arch=any
 libva-wayland2 deb libs optional arch=linux-any
 libva-x11-2 deb libs optional arch=any
 libva2 deb libs optional arch=any
 va-driver-all deb video optional arch=any
Checksums-Sha1:
 b18e281f204f81c2d3d263b061c8b27211fc0300 211632 libva_2.20.0.orig.tar.xz
 6e4928868398f3c56e30dd22955fe1b048410b7e 12308 libva_2.20.0-2build1.debian.tar.xz
Checksums-Sha256:
 a19cbc3b537cd04f43a4511448bcd9fd0b065ea9a3f83efd590bac0a567b0752 211632 libva_2.20.0.orig.tar.xz
 5035392b8ed00f9c2a59d60d844138dd2c03e55d2216019f3f559029e8edefd5 12308 libva_2.20.0-2build1.debian.tar.xz
Files:
 172fe917204bec9193e0522361b2a821 211632 libva_2.20.0.orig.tar.xz
 7fd346b1202f4212f4ee3f8f32563ede 12308 libva_2.20.0-2build1.debian.tar.xz
Original-Maintainer: Debian Multimedia Maintainers <debian-multimedia@lists.debian.org>

-----BEGIN PGP SIGNATURE-----

iQJHBAEBCgAxFiEET7WIqEwt3nmnTHeHb6RY3R2wP3EFAmYhXPYTHGp1bGlhbmtA
dWJ1bnR1LmNvbQAKCRBvpFjdHbA/caSVD/oDq5yT8LEpdpq01CsuizKqpkHg6SvC
JiVDNOlvlF1yVYH91x+bKDqnHU0Kx/UwojnxhKviRzfZaSH2mp7SktE+whv4hMNc
PtyP1rWinktuAoloeYJC30SmLvRKap5Z9W0MkwP0Qo9o46zScJuphW38OFmNC9dS
/TLQCK9QonaAazDkjMKCSQn1+u96Ep/ysbMgGFbS7YK1TMta1eQ4EcvuXUEKq4vk
xvi4HXGY+pQsHLkkkl0Nrqg7ppLgFzWWgVHcrgcYZOTZr/LeIaCL54zQ+GOy8Aap
OG3wFIK2/0Esvsohy+a0l2a9yPuhG2mNdexOWbtSSB/RymAVwEDnI02y0Nxixbuv
7Cngx+7OsWiS47uVYNBYMpKsknHMszs4OvD4ShheZzcvF49bPlFG4+kTPqpDysTA
4vXCHaBT7LSM3fSdZyaKztwan3XcpBbKRCNnjina5JXwgvhyznEtsWx6jPm4ru8z
YlFrrJEijJqeYVF7T1MLqfC5qSXlAOb6PUC+3jdUf9V734/l7pGbKt7ocm2SIzdF
WzykLhxvbBNpcCbDx1q208sVsYKnBe/IhTSKD62gU+KTCVIv0FTNafbD80fBZlvo
x+/7eKjb3McvzB5gmE55B3NqVeZZIfkrVg0EpywzkFYe8mtZtoYGNWnSV+GxZZUL
Zvy7+KId5NETxw==
=pKuj
-----END PGP SIGNATURE-----
