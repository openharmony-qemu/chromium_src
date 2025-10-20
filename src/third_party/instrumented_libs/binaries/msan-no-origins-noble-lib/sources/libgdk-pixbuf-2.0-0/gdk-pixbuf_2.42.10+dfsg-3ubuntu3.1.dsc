-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: gdk-pixbuf
Binary: libgdk-pixbuf-2.0-0, libgdk-pixbuf2.0-bin, libgdk-pixbuf2.0-common, libgdk-pixbuf-2.0-dev, libgdk-pixbuf2.0-doc, libgdk-pixbuf2.0-0-udeb, libgdk-pixbuf-2.0-0-udeb, gir1.2-gdkpixbuf-2.0, gdk-pixbuf-tests
Architecture: any all
Version: 2.42.10+dfsg-3ubuntu3.1
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Emilio Pozuelo Monfort <pochu@debian.org>, Iain Lane <laney@debian.org>, Jeremy Bicha <jbicha@ubuntu.com>, Laurent Bigonville <bigon@debian.org>
Homepage: https://www.gtk.org/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/gnome-team/gdk-pixbuf
Vcs-Git: https://salsa.debian.org/gnome-team/gdk-pixbuf.git
Testsuite: autopkgtest
Testsuite-Triggers: build-essential, gnome-desktop-testing, pkg-config
Build-Depends: debhelper-compat (= 13), dh-sequence-gir, gi-docgen, libgirepository1.0-dev (>= 1.78.1-5~), libglib2.0-dev, libjpeg-dev, libpng-dev (<< 1.7), libtiff-dev, meson, python3-docutils <!nodoc>, shared-mime-info, xsltproc <!noudeb>
Package-List:
 gdk-pixbuf-tests deb libs optional arch=any
 gir1.2-gdkpixbuf-2.0 deb introspection optional arch=any
 libgdk-pixbuf-2.0-0 deb libs optional arch=any
 libgdk-pixbuf-2.0-0-udeb udeb debian-installer optional arch=any profile=!noudeb
 libgdk-pixbuf-2.0-dev deb libdevel optional arch=any
 libgdk-pixbuf2.0-0-udeb udeb debian-installer optional arch=any profile=!noudeb
 libgdk-pixbuf2.0-bin deb libs optional arch=any
 libgdk-pixbuf2.0-common deb libs optional arch=all
 libgdk-pixbuf2.0-doc deb doc optional arch=all profile=!nodoc
Checksums-Sha1:
 08baf45662714b21a1fa78d1ade4926cee1a5506 6439240 gdk-pixbuf_2.42.10+dfsg.orig.tar.xz
 41894ce6a91744d2b5988b1a7ed5445d6f0245e2 28372 gdk-pixbuf_2.42.10+dfsg-3ubuntu3.1.debian.tar.xz
Checksums-Sha256:
 46663e445468e92f4a0ca876b02aed4f8758595ee3acfaa6ef3ba2b29e1c1930 6439240 gdk-pixbuf_2.42.10+dfsg.orig.tar.xz
 5e750bbde6d0299c64ced29b766cc67365695daa17df02ee90a19b6b2b76acc4 28372 gdk-pixbuf_2.42.10+dfsg-3ubuntu3.1.debian.tar.xz
Files:
 25dc1bf2c14ae78161f603fe62dad38f 6439240 gdk-pixbuf_2.42.10+dfsg.orig.tar.xz
 9f0006a5e4fd78fa4cdff6b042d78bfc 28372 gdk-pixbuf_2.42.10+dfsg-3ubuntu3.1.debian.tar.xz
Original-Maintainer: Debian GNOME Maintainers <pkg-gnome-maintainers@lists.alioth.debian.org>

-----BEGIN PGP SIGNATURE-----

iQHRBAEBCgA7FiEEcxdv4gCCE8W9nrt5a1+PL+d1/EgFAmZeBDcdHGlhbi5jb25z
dGFudGluQGNhbm9uaWNhbC5jb20ACgkQa1+PL+d1/EivYAwAkAnSgnv25ZARnptl
nhyrZg1emv9mxWY6HMoLej77CKcienZ6HXQVoTt9hqIbomJSOoLvZ4OmnQaW7CvV
cbxyjz7an5IBRuwU69qQgbbUQ06QYDI5r4kgk6MYt98VmhYwCanQmxUIoaVn5T40
doAeQ4j+J1c9uHLHDEp5OLpiplfkjHPws0uQPFaECXfeBIDf/6iYz9f4IacVFE0Y
VAuyb0GhPb5NYz1DVQgNcSSxd/02dm8E1Q14nP68R5Dt2uXe+t7P0lV3ancubScV
ppjEZjzwav6O35shTF0Q+M3SmM2UqFLsH6xhLuJEW47SUERLHgdckZIlH0sM8M1q
LwDZ+gMOdfuhWQc7bqqEyUf6JTzVxeN4lkYGfHds0XBkG5FTqNNwTSLm9ujNAhtU
SVtiV5mzeLE/8UQfRvPdJhq49lx1+eClemDZy3bdNsQpBS/lqK4Bi+rzjrXCvUeE
V62MVFwNEiaDB0WUpR4mxdBnCr2Frinm5rOvQDc5j/dDcr9A
=4Q4K
-----END PGP SIGNATURE-----
