-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: brltty
Binary: brltty, libbrlapi0.8, libbrlapi-dev, libbrlapi-jni, libbrlapi-java, brltty-flite, brltty-speechd, brltty-espeak, brltty-udeb, brltty-x11, xbrlapi, cl-brlapi, python3-brlapi
Architecture: any all
Version: 6.6-4ubuntu5
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Samuel Thibault <sthibault@debian.org>
Homepage: https://brltty.com
Standards-Version: 4.6.0
Vcs-Browser: https://salsa.debian.org/a11y-team/brltty
Vcs-Git: https://salsa.debian.org/a11y-team/brltty.git
Build-Depends: debhelper-compat (= 12), dh-lisp, dh-python, dh-strip-nondeterminism, dh-exec, pkg-config, tcl, libasound2-dev [linux-any], python3-all-dev:any, python3-setuptools, cython3, libexpat1-dev, libgpm-dev [linux-any], libsystemd-dev [linux-any], libicu-dev, liblouis-dev, libpolkit-gobject-1-dev, libcap-dev [linux-any], default-jdk [!hppa !hurd-any !kfreebsd-any], doxygen, linuxdoc-tools, groff, flite1-dev, libncurses-dev, libx11-dev, libxt-dev, libxaw7-dev, x11proto-kb-dev, libxtst-dev, libxfixes-dev, libdbus-1-dev, libatspi2.0-dev, libglib2.0-dev, libbluetooth-dev [linux-any], libspeechd-dev, libespeak-ng-dev, libespeak-ng-libespeak-dev, po-debconf, gettext
Build-Conflicts: libasound2-dev [!linux-any], libatspi-dev
Package-List:
 brltty deb admin optional arch=any
 brltty-espeak deb admin optional arch=any
 brltty-flite deb admin optional arch=any
 brltty-speechd deb admin optional arch=any
 brltty-udeb udeb debian-installer optional arch=any profile=!noudeb
 brltty-x11 deb admin optional arch=any
 cl-brlapi deb lisp optional arch=all
 libbrlapi-dev deb libdevel optional arch=any
 libbrlapi-java deb java optional arch=all
 libbrlapi-jni deb java optional arch=any
 libbrlapi0.8 deb libs optional arch=any
 python3-brlapi deb python optional arch=any
 xbrlapi deb admin optional arch=any
Checksums-Sha1:
 9dce2aa507abdc5588dbed1ed721fd502c5dc0be 4115292 brltty_6.6.orig.tar.xz
 1340ed06e31576477d1ac8e0c26fc91f90cba3cd 42532 brltty_6.6-4ubuntu5.debian.tar.xz
Checksums-Sha256:
 b58da402b122d5e135b4131b7fde07ada518f9aedcca721f81e21e192dfad637 4115292 brltty_6.6.orig.tar.xz
 517e4523fa551ed981c02b938ed3c723dfc973da62988bbf0012a7321bad5a31 42532 brltty_6.6-4ubuntu5.debian.tar.xz
Files:
 58ca5e0713d80f599ebeb08f6d4995b6 4115292 brltty_6.6.orig.tar.xz
 63022e535194de32defbd3fd464f17af 42532 brltty_6.6-4ubuntu5.debian.tar.xz
Original-Maintainer: Debian Accessibility Team <pkg-a11y-devel@alioth-lists.debian.net>

-----BEGIN PGP SIGNATURE-----

iQJOBAEBCgA4FiEErEg/aN5yj0PyIC/KVo0w8yGyEz0FAmYIrTEaHHN0ZXZlLmxh
bmdhc2VrQHVidW50dS5jb20ACgkQVo0w8yGyEz2jNA//QjYngohKeejo5Eu3ZiiO
Q9bokg4SMoK608a6EOEntWrXvYm/6FypFHp+k9BcVH0BXx51pdicv6RiOyMUdlas
kjplqTHLoDgf2Qx3V+ihhX6zWcfKeQwpAzTWjEuh6PyKYYgJAXPll6qiucvXHEqG
391vm/ihmnV2OUIQlnta/n1e9/C5rBZJxFAGME31rQfaBQE0cQowP7vYIiF2Ftkx
nY096Lu7NI2rJwgRQ99tXoR2q52h0O1eKU/Pd9SQYISfHGbf9fYtTQL8irdcVc02
OpXPnwIya7M0XoDXKRvfocY1SNZoRjvCZVLAxUPri3q8IGaw8pWO6nOa9FcMiyha
nc2dsvO9G6sEYn6JD67qWhKKD/4NC6Hn+82PgFEUG/94IxsdjQ/dCFBv8qttYk2J
r2jEs7hYIogF049KRD5UaViGv9jN57OJXw02kGuIQB64Gc2lGyRS4ZYI88D+y8Fw
/rt0yPl9PClmeXwLcs/oS956WW9NEf479mjOv5I9aCLYXDID8BvmnZm7XRMWOMoU
OO6Z90j8ZTgDc9jQPJpaiWoyDQc+uNfreBIL4R6zkC59lDnmvbOK+5DJFGSTSjUJ
dR0AbSk3Nj+dKURDujM4v3DaZU/k+fBGiP+YsTUNfcSVdX+V16012rlkOf/zJg4W
RGIo6jnMl8/VVMz5mUlnTVY=
=UZAV
-----END PGP SIGNATURE-----
