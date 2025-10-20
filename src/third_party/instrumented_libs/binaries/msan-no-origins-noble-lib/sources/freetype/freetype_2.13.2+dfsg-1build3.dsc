-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: freetype
Binary: libfreetype6, libfreetype-dev, freetype2-demos, freetype2-doc, libfreetype6-udeb
Architecture: any all
Version: 2.13.2+dfsg-1build3
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Anthony Fok <foka@debian.org>, Keith Packard <keithp@keithp.com>
Homepage: https://freetype.org
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/debian/freetype
Vcs-Git: https://salsa.debian.org/debian/freetype.git
Testsuite: autopkgtest
Testsuite-Triggers: build-essential, pkg-config
Build-Depends: debhelper-compat (= 13), autoconf, bzip2, gettext, libbrotli-dev, libbz2-dev, libpng-dev, libtool, libx11-dev <!pkg.freetype.nodemos>, zlib1g-dev | libz-dev, pkg-config, x11proto-core-dev <!pkg.freetype.nodemos>
Package-List:
 freetype2-demos deb utils optional arch=any profile=!pkg.freetype.nodemos
 freetype2-doc deb doc optional arch=all
 libfreetype-dev deb libdevel optional arch=any
 libfreetype6 deb libs optional arch=any
 libfreetype6-udeb udeb debian-installer optional arch=any profile=!noudeb
Checksums-Sha1:
 655c82a431fae7f53a964bda8a7c0671531d05a4 341140 freetype_2.13.2+dfsg.orig-ft2demos.tar.xz
 ca7f1fd515f05b34d41f0438bcce3c0e7559ec67 833 freetype_2.13.2+dfsg.orig-ft2demos.tar.xz.asc
 dbed086b3dba1d748e15b28103081ed30d24e3f3 2173920 freetype_2.13.2+dfsg.orig-ft2docs.tar.xz
 645b5dee18441e95efaa790d2cc14c092f4921e2 833 freetype_2.13.2+dfsg.orig-ft2docs.tar.xz.asc
 f77455e0d78de99ad307ac2ca282cbd32e318316 2220368 freetype_2.13.2+dfsg.orig.tar.xz
 efdd83718923410fe09ad540cc739915924960a3 44000 freetype_2.13.2+dfsg-1build3.debian.tar.xz
Checksums-Sha256:
 99ee2ed8b98bcfad17bc57c2d9699d764f20fe29ad304c69b8eb28834ca3b48e 341140 freetype_2.13.2+dfsg.orig-ft2demos.tar.xz
 e58ba462f7bdcdc5899f777d33453c1ce6f6e46b080047580f45c9fd9f2dc08c 833 freetype_2.13.2+dfsg.orig-ft2demos.tar.xz.asc
 685c25e1035a5076e5097186b3143b9c06878f3f9087d0a81e4d8538d5d15424 2173920 freetype_2.13.2+dfsg.orig-ft2docs.tar.xz
 d7e17c8a3bce50181530ebe06346f506cbfc92ecc5ca7cc395c7dbb24a71a5c0 833 freetype_2.13.2+dfsg.orig-ft2docs.tar.xz.asc
 48c78a4194adfcd15a4d089f3206dab8454c311f5577f3ef7eaef95f777f86e6 2220368 freetype_2.13.2+dfsg.orig.tar.xz
 67d83f620d3b8b1209d15ee3d96b1e5847c0f629330cf6a74b40fb938fd5bd07 44000 freetype_2.13.2+dfsg-1build3.debian.tar.xz
Files:
 e11243fb7828d2f151409720735cc451 341140 freetype_2.13.2+dfsg.orig-ft2demos.tar.xz
 c7f036af01a616f2fb2ec7e514048bae 833 freetype_2.13.2+dfsg.orig-ft2demos.tar.xz.asc
 e6466f08934ebf2d7f711b0464d7a6cf 2173920 freetype_2.13.2+dfsg.orig-ft2docs.tar.xz
 0a164c49b6e43db4d5b9b7b73dd2a5e7 833 freetype_2.13.2+dfsg.orig-ft2docs.tar.xz.asc
 7d938e51320c4cf50e2203d0c325d925 2220368 freetype_2.13.2+dfsg.orig.tar.xz
 e1a29a49a7830e003b9dfacaffffd245 44000 freetype_2.13.2+dfsg-1build3.debian.tar.xz
Original-Maintainer: Hugh McMaster <hmc@debian.org>

-----BEGIN PGP SIGNATURE-----

iQJOBAEBCgA4FiEErEg/aN5yj0PyIC/KVo0w8yGyEz0FAmYI82QaHHN0ZXZlLmxh
bmdhc2VrQHVidW50dS5jb20ACgkQVo0w8yGyEz3PeQ//cWNnIhuGq/KVth429uSK
mNfhgAND1OgaOeicgZtpVfhG9ef0EGU3lwO1ZF+/3E9mzc4/9Ujnaxbq5R4LZTJ5
CzWNY2VoDfoKBGNOZU7G+Mms2TeJlTCL8gXp3814ZvAxVVOgmU3Gf548Ou++YsBu
7cNRNGiyRLFJTFXmF/fJbljAi6qOtMXdez6muGTSrftUoiLPrtRP0/eFLlxwKg62
dXwmVC20EJCrZeYkqEG3ZjO/X1slCosCnyGWPENkz8xZBCx3FFBZ1Rqh3RUAyYXa
yACxGHQGG1DTgJ95QjzjAxUxStAeGMXSVF3FjTB/JlHzIrCUNEdyUW5upnsaQO9v
vzsk9yZ3dfNZKyv4uCDC83oKnOzAC0HrJJtQH8io5cNEvgNPT0OvX5A4IdLYzmcI
DbjhBWZoFUtffF/6Y1OqUI3bjuTi71tGA1n3Bga1JGGbKdY87naAO2x8H99hl1ze
aQE2j24YDj2IY2Cf9fA+q5V0NG1FNHtZ7NlUtF6cjDsb/4O40Xu5ZZQlC7WmiVH0
S2X+Q5z+zt6mE2kIQbWH7MiJuHCxIrSVMmq8vZhRsXhz1lrLGGv94ymjuKNDImBG
XOisWvYygdF5RH6+COwoIMtotl7aU+lihJ6x7vWpq29Ox1jVMB6uGMYr515K9NSd
T4NPLnmr6jrZxnhQAlNmIIg=
=wFu9
-----END PGP SIGNATURE-----
