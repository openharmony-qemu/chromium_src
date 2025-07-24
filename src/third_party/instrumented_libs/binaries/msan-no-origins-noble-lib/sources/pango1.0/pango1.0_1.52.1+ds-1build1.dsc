-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: pango1.0
Binary: libpango-1.0-0, libpangocairo-1.0-0, libpangoxft-1.0-0, libpangoft2-1.0-0, libpango1.0-udeb, libpango1.0-dev, libpango1.0-doc, gir1.2-pango-1.0, pango1.0-tests, pango1.0-tools
Architecture: any all
Version: 1.52.1+ds-1build1
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Emilio Pozuelo Monfort <pochu@debian.org>, Iain Lane <laney@debian.org>, Jeremy Bícha <jbicha@ubuntu.com>, Laurent Bigonville <bigon@debian.org>, Marco Trevisan (Treviño) <marco@ubuntu.com>, Michael Biebl <biebl@debian.org>
Homepage: https://www.pango.org/
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/gnome-team/pango
Vcs-Git: https://salsa.debian.org/gnome-team/pango.git
Testsuite: autopkgtest
Testsuite-Triggers: build-essential, dbus, gir1.2-gdk-3.0, gnome-desktop-testing, locales, locales-all, pkg-config, python3-gi, xauth, xvfb
Build-Depends: debhelper-compat (= 13), dh-sequence-gir, fonts-cantarell <!nocheck>, fonts-dejavu-core <!nocheck>, fonts-noto-color-emoji <!nocheck>, fonts-noto-core <!nocheck>, gir1.2-cairo-1.0-dev, gir1.2-gio-2.0-dev, gir1.2-gobject-2.0-dev, gir1.2-harfbuzz-0.0-dev <pkg.gobject-introspection.todo>, gobject-introspection (>= 1.78.1-9~), help2man, libcairo2-dev, libfontconfig-dev, libfreetype-dev, libfribidi-dev, libglib2.0-dev, libharfbuzz-dev, libthai-dev, libx11-dev, libxft-dev, libxrender-dev, libxt-dev, locales <!nocheck> | locales-all <!nocheck>, meson, perl:native, pkgconf
Build-Depends-Indep: gi-docgen <!nodoc>
Package-List:
 gir1.2-pango-1.0 deb introspection optional arch=any
 libpango-1.0-0 deb libs optional arch=any
 libpango1.0-dev deb libdevel optional arch=any
 libpango1.0-doc deb doc optional arch=all profile=!nodoc
 libpango1.0-udeb udeb debian-installer optional arch=any
 libpangocairo-1.0-0 deb libs optional arch=any
 libpangoft2-1.0-0 deb libs optional arch=any
 libpangoxft-1.0-0 deb libs optional arch=any
 pango1.0-tests deb libs optional arch=any
 pango1.0-tools deb libs optional arch=any profile=nodoc+!cross
Checksums-Sha1:
 2290f5c83bb7ba4e2f55e884c8bb684553ad8989 1738052 pango1.0_1.52.1+ds.orig.tar.xz
 5ad62e1c7ea8c772b5e6345c3c2f3e0894fc8fb7 41748 pango1.0_1.52.1+ds-1build1.debian.tar.xz
Checksums-Sha256:
 12d67d8182cbb2ae427406df9bab5ce2ff5619102bf2a0fc6331d80a9914b139 1738052 pango1.0_1.52.1+ds.orig.tar.xz
 688c01a1af76bfb861805bbd66cddeac6a968123b10e401877974a8d9b8ae236 41748 pango1.0_1.52.1+ds-1build1.debian.tar.xz
Files:
 b30da7d093a0546f97cf071bf7b08009 1738052 pango1.0_1.52.1+ds.orig.tar.xz
 15584c178afa8b76f598419e74cf61d0 41748 pango1.0_1.52.1+ds-1build1.debian.tar.xz
Original-Maintainer: Debian GNOME Maintainers <pkg-gnome-maintainers@lists.alioth.debian.org>

-----BEGIN PGP SIGNATURE-----

iQJOBAEBCgA4FiEErEg/aN5yj0PyIC/KVo0w8yGyEz0FAmYJBiwaHHN0ZXZlLmxh
bmdhc2VrQHVidW50dS5jb20ACgkQVo0w8yGyEz04ShAAxh0bX96K9D7p3CqiX8/I
ugLDbha2lf4uUhf86dFyg/N25KKyfRF69EOMK1L7wgfgkKf+7gmDSqCGePFp9iYL
q7AMUGsXOpP46r0KG4ZqCNnGqeyDtn2f/JeYneH9qo7zir1K9UDRZRj6JXhQbChm
5qieUaxOCnNPQtP+I2D8BjnDgaR3SHgFT18U06mUPUG1BN6izmsqxLKsCpX2mzCc
xphxhTW+5ongu8ROu+O8/V6z96IY9O3mSndz2gKkFY8dG8pvf16jxKyfkLeANPB8
Rzm/eOQ0ofyTjxAwJA6sDgmvzhovsDJFWvOcxmjUK5BWWzSepAf8QGYuvc3scQna
B3AfFxVuv7LQiZB3NLr/UFVQEbE5bqtg02+f+Im0te/sZtIGfLgcK+EXrqtrcqOp
FBrfQ2ol2XWhv7BIRGNZTa1HHlT4lo56nMDJcMKanAnb5UUkMOMKBsVQ9P33+ESn
yFCG6A2afUqG8G036b9ceZbjkXP4lFFAJViVnaNRY8C0TsKQ64ooIaV2lszzH2xe
r9WJqvZ3uurseH2NkYTHy7w/UExXQdkcOHOxpt0rtVXCB5idMHEAKirGwv1Mf5o1
baJXQnPc9JFJwu5xWLQuhKoJgWYkTFij8MKl4lDkAEFhBZS4UqnB6K4J+4eZ8o5F
6AqS6zVR3nzhqmOe9NXX2Ws=
=l54G
-----END PGP SIGNATURE-----
