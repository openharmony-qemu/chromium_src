-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: zlib
Binary: zlib1g, zlib1g-dev, zlib1g-udeb, lib64z1, lib64z1-dev, lib32z1, lib32z1-dev, libn32z1, libn32z1-dev, libx32z1, libx32z1-dev, minizip, libminizip1t64, libminizip-dev
Architecture: any
Version: 1:1.3.dfsg-3.1ubuntu2
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Homepage: http://zlib.net/
Standards-Version: 4.6.1
Build-Depends: dpkg-dev (>= 1.22.5), debhelper (>= 13), gcc-multilib [amd64 i386 kfreebsd-amd64 mips mipsel powerpc ppc64 s390 sparc s390x mipsn32 mipsn32el mipsr6 mipsr6el mipsn32r6 mipsn32r6el mips64 mips64el mips64r6 mips64r6el x32] <!nobiarch>, autoconf
Package-List:
 lib32z1 deb libs optional arch=amd64,ppc64,kfreebsd-amd64,s390x profile=!nobiarch
 lib32z1-dev deb libdevel optional arch=amd64,ppc64,kfreebsd-amd64,s390x profile=!nobiarch
 lib64z1 deb libs optional arch=sparc,s390,i386,powerpc,mips,mipsel,mipsn32,mipsn32el,mipsr6,mipsr6el,mipsn32r6,mipsn32r6el,x32 profile=!nobiarch
 lib64z1-dev deb libdevel optional arch=sparc,s390,i386,powerpc,mips,mipsel,mipsn32,mipsn32el,mipsr6,mipsr6el,mipsn32r6,mipsn32r6el,x32 profile=!nobiarch
 libminizip-dev deb libdevel optional arch=any
 libminizip1t64 deb libs optional arch=any
 libn32z1 deb libs optional arch=mips,mipsel profile=!nobiarch
 libn32z1-dev deb libdevel optional arch=mips,mipsel profile=!nobiarch
 libx32z1 deb libs optional arch=amd64,i386
 libx32z1-dev deb libdevel optional arch=amd64,i386
 minizip deb utils optional arch=any
 zlib1g deb libs required arch=any
 zlib1g-dev deb libdevel optional arch=any
 zlib1g-udeb udeb debian-installer optional arch=any
Checksums-Sha1:
 1013da3e3125946039f2c60e291a54a9422b4553 1128572 zlib_1.3.dfsg.orig.tar.xz
 53545cab3d9a766f305cc6a4b165180369f310cd 60796 zlib_1.3.dfsg-3.1ubuntu2.debian.tar.xz
Checksums-Sha256:
 5eea0322c1c21c75cad3b607ac1c43ff5c71e014b8ac4a34300b5e2b80d02e70 1128572 zlib_1.3.dfsg.orig.tar.xz
 645a2ecc2a3c1d263784717ba7f5ad6672261979523461a4d6cdbed217caae59 60796 zlib_1.3.dfsg-3.1ubuntu2.debian.tar.xz
Files:
 3eee75fc36a344cfe93c8bf1c0473df4 1128572 zlib_1.3.dfsg.orig.tar.xz
 063d27f40fdc4e2d962538da4d9bd95f 60796 zlib_1.3.dfsg-3.1ubuntu2.debian.tar.xz
Original-Maintainer: Mark Brown <broonie@debian.org>

-----BEGIN PGP SIGNATURE-----

iQJOBAEBCgA4FiEErEg/aN5yj0PyIC/KVo0w8yGyEz0FAmYJIGYaHHN0ZXZlLmxh
bmdhc2VrQHVidW50dS5jb20ACgkQVo0w8yGyEz2hvxAAvxMzB29I2t7Zvz4P+PBO
HquI0rfFlTdWZ7XbLR8uLmuAWBSVZNF564uaStzCVzStCQSctJt9kAiAibShOvOK
Pv37UfD7v/ipnoCdEQI6kGzC8R9M0KiEVmoA45gSsAGL8ahRsiJUNyne5GlZjF0I
cVbvBfRx8Ry2xzxzM7rNusID2XMCBIoBPBzy0j2f0g8a3odsvZ10rvrXCocvJpWt
377bZfOuQjb9zL9VN+xxe0aC4BoNiQGfMQMZnsStnbCuNDEBaEVwqNg5aQUh05XI
kTKIUYyhQ9YT2wgF8OnsymCVY/uWpgr2YMaoCMoUMBGg1F+FrI+sGF5RZhn2EsAC
Jw9IwXm8pWEukgnsqDnowqrdYhE+jKZuiKGCcZlnrvZTiX2A7M9VPp60HO+72nXw
iL8++yGV6eTM7jpTdb7j5oTJIbdC1pHRQ/Kkf5O13r15lyj/Nd19++pFvyUWdEQj
B2I3inEOJtMMQtpWMnB1TsDpEkrMJ7+HMKnRZh+YGIy1F2OfJVgOcYkG7cGh3ZSR
cbJEi9b8MkNA7km6jYIK4OC5gsKJOOGofuZ7HFVn8EgQoQol8/EkgFet5J2I1+DD
8yGklfT9OB726reEQLwwYNqNFcN9SFLfkovz6Mtruf6F7e2x0IyE1o5ZrMV8rEcw
LHNvNRgJZjSOyVHz9h3dJBE=
=Kb8d
-----END PGP SIGNATURE-----
