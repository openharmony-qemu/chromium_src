-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 1.0
Source: libxcb
Binary: libxcb1, libxcb1-udeb, libxcb1-dev, libxcb-doc, libxcb-composite0, libxcb-composite0-dev, libxcb-damage0, libxcb-damage0-dev, libxcb-dpms0, libxcb-dpms0-dev, libxcb-glx0, libxcb-glx0-dev, libxcb-randr0, libxcb-randr0-dev, libxcb-record0, libxcb-record0-dev, libxcb-render0, libxcb-render0-dev, libxcb-res0, libxcb-res0-dev, libxcb-screensaver0, libxcb-screensaver0-dev, libxcb-shape0, libxcb-shape0-dev, libxcb-shm0, libxcb-shm0-dev, libxcb-sync1, libxcb-sync-dev, libxcb-xf86dri0, libxcb-xf86dri0-dev, libxcb-xfixes0, libxcb-xfixes0-dev, libxcb-xinerama0, libxcb-xinerama0-dev, libxcb-xinput0, libxcb-xinput-dev, libxcb-xtest0, libxcb-xtest0-dev, libxcb-xv0, libxcb-xv0-dev, libxcb-xvmc0, libxcb-xvmc0-dev, libxcb-dri2-0, libxcb-dri2-0-dev, libxcb-present0, libxcb-present-dev, libxcb-dri3-0, libxcb-dri3-dev, libxcb-xkb1, libxcb-xkb-dev
Architecture: any all
Version: 1.15-1ubuntu2
Maintainer: Debian X Strike Force <debian-x@lists.debian.org>
Uploaders:  Julien Cristau <jcristau@debian.org>,
Homepage: https://xcb.freedesktop.org
Standards-Version: 4.5.0
Vcs-Browser: https://salsa.debian.org/xorg-team/lib/libxcb
Vcs-Git: https://salsa.debian.org/xorg-team/lib/libxcb.git
Testsuite: autopkgtest
Testsuite-Triggers: build-essential, pkg-config, xauth, xvfb
Build-Depends: libxau-dev (>= 1:1.0.5-2), libxdmcp-dev (>= 1:1.0.3-2), xcb-proto (>= 1.15), xcb-proto (<< 2.0), libpthread-stubs0-dev (>= 0.1), debhelper-compat (= 12), pkg-config, xutils-dev, xsltproc (>= 1.1.19), check (>= 0.9.4-2) <!nocheck>, python3-xcbgen (>= 1.14), libtool, automake, python3:native, dctrl-tools
Build-Depends-Indep: doxygen, graphviz
Package-List:
 libxcb-composite0 deb libs optional arch=any
 libxcb-composite0-dev deb libdevel optional arch=any
 libxcb-damage0 deb libs optional arch=any
 libxcb-damage0-dev deb libdevel optional arch=any
 libxcb-doc deb doc optional arch=all
 libxcb-dpms0 deb libs optional arch=any
 libxcb-dpms0-dev deb libdevel optional arch=any
 libxcb-dri2-0 deb libs optional arch=any
 libxcb-dri2-0-dev deb libdevel optional arch=any
 libxcb-dri3-0 deb libs optional arch=any
 libxcb-dri3-dev deb libdevel optional arch=any
 libxcb-glx0 deb libs optional arch=any
 libxcb-glx0-dev deb libdevel optional arch=any
 libxcb-present-dev deb libdevel optional arch=any
 libxcb-present0 deb libs optional arch=any
 libxcb-randr0 deb libs optional arch=any
 libxcb-randr0-dev deb libdevel optional arch=any
 libxcb-record0 deb libs optional arch=any
 libxcb-record0-dev deb libdevel optional arch=any
 libxcb-render0 deb libs optional arch=any
 libxcb-render0-dev deb libdevel optional arch=any
 libxcb-res0 deb libs optional arch=any
 libxcb-res0-dev deb libdevel optional arch=any
 libxcb-screensaver0 deb libs optional arch=any
 libxcb-screensaver0-dev deb libdevel optional arch=any
 libxcb-shape0 deb libs optional arch=any
 libxcb-shape0-dev deb libdevel optional arch=any
 libxcb-shm0 deb libs optional arch=any
 libxcb-shm0-dev deb libdevel optional arch=any
 libxcb-sync-dev deb libdevel optional arch=any
 libxcb-sync1 deb libs optional arch=any
 libxcb-xf86dri0 deb libs optional arch=any
 libxcb-xf86dri0-dev deb libdevel optional arch=any
 libxcb-xfixes0 deb libs optional arch=any
 libxcb-xfixes0-dev deb libdevel optional arch=any
 libxcb-xinerama0 deb libs optional arch=any
 libxcb-xinerama0-dev deb libdevel optional arch=any
 libxcb-xinput-dev deb libdevel optional arch=any
 libxcb-xinput0 deb libs optional arch=any
 libxcb-xkb-dev deb libdevel optional arch=any
 libxcb-xkb1 deb libs optional arch=any
 libxcb-xtest0 deb libs optional arch=any
 libxcb-xtest0-dev deb libdevel optional arch=any
 libxcb-xv0 deb libs optional arch=any
 libxcb-xv0-dev deb libdevel optional arch=any
 libxcb-xvmc0 deb libs optional arch=any
 libxcb-xvmc0-dev deb libdevel optional arch=any
 libxcb1 deb libs optional arch=any
 libxcb1-dev deb libdevel optional arch=any
 libxcb1-udeb udeb debian-installer optional arch=any
Checksums-Sha1:
 f6d149e5e7ada566e31516859eb6b69e75426217 650774 libxcb_1.15.orig.tar.gz
 eb6d731fa649e54537f5479c9c3312e94e269178 26975 libxcb_1.15-1ubuntu2.diff.gz
Checksums-Sha256:
 1cb65df8543a69ec0555ac696123ee386321dfac1964a3da39976c9a05ad724d 650774 libxcb_1.15.orig.tar.gz
 d45e55f604af83b47f621c97b49885f35b518909bb3b9f9c877cfd3daddfb5ee 26975 libxcb_1.15-1ubuntu2.diff.gz
Files:
 0254c1e455d1d49f778e687aacc93368 650774 libxcb_1.15.orig.tar.gz
 a7179df01834f24dc6c62d07702d7b72 26975 libxcb_1.15-1ubuntu2.diff.gz

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCgAdFiEEdS3ifE3rFwGbS2Yjy3AxZaiJhNwFAmYhLIwACgkQy3AxZaiJ
hNxOThAAi5k0JRW3vv5LQOrPx0btY7L5SDBzwTGLkOBvINWmgqR4D5Wwu7xC/FNw
8jIQDtqeMod8LT7dna7X7IL5aVwGUs6fHiU7Xo0llBqbjF33ATN9Vk8Wb/yeFwdZ
yRd8ymYn0lpj1KxeFq0mXSybZnQQJdsTjItqOK03Ad82w2T5yWdqEtETlaET4N/7
1J8wBEjjGyNhv8rNPGJ2NYpYyBkLjfazASeCMUOmr90eQFIoh16edy6SfjTclBYH
2jOyQlW35FAqhxGrVgjxIHjDWnedrt3x5YXZL7oPlgvP6w+sJHpW6SAecytwrD2g
dw2mg7xBsbMaBMECRkp8XxX1VOIg40t8HpnSLVeu5qMLU2Bn9FtLlAXkmZQ8yS6Q
PAcXVNODxBvKccAnCbGgekkShpfUdzR+thTzvvGBGydRi4W6MxrCjlK6HBlM4xsS
LUVMch2xQppXt8GZTHlG4etVVh+i66xkdySSg7mgdW3GFbl/l7CsTmciRiQt8O4M
E1nx7YIQ1I4HVA18EUAlpgn2MtMRxyFQm/yUhY9DQpfXfkyZTHFyqf4cUAU5afV3
oPA+Y0oHkCGsm3cWo0z3fLO7ivMLwLh6Oqebyhpyn6Z2cQYv6NkC+3N10mRQCbkZ
BZq9qWPkcvBdzDKaC5lqolfPISBzM5foWsy9m3dTo5d6vUrtysQ=
=MavN
-----END PGP SIGNATURE-----
