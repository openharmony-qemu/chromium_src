-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: glib2.0
Binary: libglib2.0-0t64, libglib2.0-tests, libglib2.0-udeb, libglib2.0-bin, libglib2.0-dev, libglib2.0-dev-bin, libglib2.0-data, libglib2.0-doc, libgirepository-2.0-0, libgirepository-2.0-dev, gir1.2-glib-2.0, gir1.2-glib-2.0-dev, gir1.2-girepository-3.0, gir1.2-girepository-3.0-dev
Architecture: any all
Version: 2.80.0-6ubuntu3.1
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Jeremy Bícha <jbicha@ubuntu.com>, Marco Trevisan (Treviño) <marco@ubuntu.com>, Simon McVittie <smcv@debian.org>
Homepage: https://wiki.gnome.org/Projects/GLib
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/gnome-team/glib
Vcs-Git: https://salsa.debian.org/gnome-team/glib.git
Testsuite: autopkgtest
Testsuite-Triggers: build-essential, dbus-daemon, dbus-x11, dconf-gsettings-backend, dpkg-repack, gnome-desktop-testing, gsettings-desktop-schemas, locales, locales-all, xauth, xvfb
Build-Depends: dbus-daemon <!nocheck> <!noinsttest>, debhelper-compat (= 13), dh-sequence-gnome, dh-sequence-python3, docbook-xml, docbook-xsl, dpkg-dev (>= 1.22.5), gettext, libdbus-1-dev <!nocheck> <!noinsttest>, libelf-dev, libffi-dev, libmount-dev [linux-any], libpcre2-dev, libselinux1-dev [linux-any], libxml2-utils, linux-libc-dev [linux-any], meson (>= 1.2.0), pkgconf, python3-docutils <!nodoc>, python3-packaging:native, python3:native, xsltproc, zlib1g-dev
Build-Depends-Arch: desktop-file-utils <!nocheck>, dh-sequence-gir <!nogir>, gobject-introspection (>= 1.78.1-18~) <!nogir>, locales <!nocheck> | locales-all <!nocheck>, python3-dbus <!nocheck>, python3-gi <!nocheck>, qemu-user <cross !nogir>, shared-mime-info <!nocheck>, tzdata <!nocheck>, xterm <!nocheck>
Build-Depends-Indep: gi-docgen <!nodoc>, gobject-introspection (>= 1.78.1-18~) <!nodoc>, qemu-user <cross !nodoc>
Package-List:
 gir1.2-girepository-3.0 deb introspection optional arch=any profile=!nogir
 gir1.2-girepository-3.0-dev deb libdevel optional arch=any profile=!nogir
 gir1.2-glib-2.0 deb introspection optional arch=any profile=!nogir
 gir1.2-glib-2.0-dev deb libdevel optional arch=any profile=!nogir
 libgirepository-2.0-0 deb libs optional arch=any
 libgirepository-2.0-dev deb libdevel optional arch=any
 libglib2.0-0t64 deb libs optional arch=any
 libglib2.0-bin deb misc optional arch=any
 libglib2.0-data deb libs optional arch=all
 libglib2.0-dev deb libdevel optional arch=any
 libglib2.0-dev-bin deb libdevel optional arch=any
 libglib2.0-doc deb doc optional arch=all profile=!nodoc
 libglib2.0-tests deb libs optional arch=any profile=!noinsttest,!nogir
 libglib2.0-udeb udeb debian-installer optional arch=any profile=!noudeb
Checksums-Sha1:
 f9c0c1f83778f34ccfa40563d21a3535fd0485a0 263364 glib2.0_2.80.0.orig-unicode-data.tar.xz
 3416bb2ce53f451afb288e948bb7c1570307e606 5510536 glib2.0_2.80.0.orig.tar.xz
 937718a1a49afa5b96e2b3bea00073a9eb1e1315 150348 glib2.0_2.80.0-6ubuntu3.1.debian.tar.xz
Checksums-Sha256:
 38680f78a0ae6258826418cb5096c19ae3566ba8fee0a2112a0ec40056e58729 263364 glib2.0_2.80.0.orig-unicode-data.tar.xz
 8228a92f92a412160b139ae68b6345bd28f24434a7b5af150ebe21ff587a561d 5510536 glib2.0_2.80.0.orig.tar.xz
 a1913c85b2918f9879a73f4f06a229e25fae1fdd78a36ae34ec60bdb2535e775 150348 glib2.0_2.80.0-6ubuntu3.1.debian.tar.xz
Files:
 52f85a65b58be1c5f2b4d0d943cff489 263364 glib2.0_2.80.0.orig-unicode-data.tar.xz
 3a51e2803ecd22c2dadcd07d9475ebe3 5510536 glib2.0_2.80.0.orig.tar.xz
 0217d162c206c31aacba5882a40e74d5 150348 glib2.0_2.80.0-6ubuntu3.1.debian.tar.xz
Original-Maintainer: Debian GNOME Maintainers <pkg-gnome-maintainers@lists.alioth.debian.org>

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCgAdFiEEUMSg3c8x5FLOsZtRZWnYVadEvpMFAmY7tHMACgkQZWnYVadE
vpO64BAAm5MX7Uqyj4mWnYwJ3zZ6sGZA7Ugs3lS/7hclgX+labdC0wktNbu3LYGB
y6vm2frd7rN+SejC/QaZarSl0J3DaPOhWEegfArJ0afE88kZd3Zj1u/rhgXhpRsk
evtLse/FuXGJ2TqxlZxSM2x2K0kOa43HYtvt1rO0YUly+gSyEme8utk6U+2g6FxM
ab0oA07a0QQsIxgn8yA+ez9m00yjZyDsMuhKhDFM4BQcWGbICFnEgU98J5kV7Cw9
KtFM8WZWdUYGst7fBypeP8OK5FolM4JJOK1m1rmhWpJYdA2qyab0rzUzpKLmrnyk
2eBOT6O892KRMWOoHXBPL6eDv6CRRde/PaCsToPIoA/jTgzhg3eKG7PnV1wbuian
aSE1Oaknc590K7sAJKpMHlW+OsKwIP9kOgADZxwdMJMbilVg215lrJ61wwCea/LY
wY2THr7IBzwVe7Sn7/tjd9Z/BGXt3jLhdLd4KWLP6EeIvESAoC5CVXEwS5UpTiv/
1fbOrcF9rfWm+PKzhXCLitmXXyxwBPTjxIahadKqiVdMORzWfk6sdkvXXf7PG/Tt
NGC+sNXiRhfsGf1hwdTXLqK+2f+UvmwgHWY9prtv+cbjNEcaRNUcWyAhYiJHiaC1
sH+ZiUzs8xWWL68cQN5PqhNRueinrjsGYKZM3RBunZSPjtUR2NM=
=WQ0I
-----END PGP SIGNATURE-----
