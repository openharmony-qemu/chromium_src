-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA512

Format: 3.0 (quilt)
Source: libsecret
Binary: libsecret-1-dev, libsecret-1-0, libsecret-common, libsecret-tools, gir1.2-secret-1
Architecture: any all
Version: 0.21.4-1build3
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Emilio Pozuelo Monfort <pochu@debian.org>, Jeremy Bícha <jbicha@ubuntu.com>, Laurent Bigonville <bigon@debian.org>, Tim Lunn <tim@feathertop.org>
Homepage: https://wiki.gnome.org/Projects/Libsecret
Standards-Version: 4.6.2
Vcs-Browser: https://salsa.debian.org/gnome-team/libsecret
Vcs-Git: https://salsa.debian.org/gnome-team/libsecret.git
Testsuite: autopkgtest
Testsuite-Triggers: build-essential, dbus, pkg-config
Build-Depends: debhelper-compat (= 13), dh-sequence-gir, dh-sequence-gnome, gi-docgen, libglib2.0-dev (>= 2.44.0), libglib2.0-doc (>= 2.44.0), libgcrypt20-dev (>= 1.2.2), libgirepository1.0-dev (>= 1.29), meson (>= 0.50.), gtk-doc-tools (>= 1.9), valac (>= 0.17.2.12), python3-dbus <!nocheck>, python3-gi <!nocheck>, dbus <!nocheck>, gjs [amd64 arm64 armel armhf i386 mips64el mipsel ppc64el riscv64 s390x] <!nocheck>
Package-List:
 gir1.2-secret-1 deb introspection optional arch=any
 libsecret-1-0 deb libs optional arch=any
 libsecret-1-dev deb libdevel optional arch=any
 libsecret-common deb libs optional arch=all
 libsecret-tools deb admin optional arch=any
Checksums-Sha1:
 94df91594f14224e1aa02bfe8d15dcc657298cc1 206636 libsecret_0.21.4.orig.tar.xz
 8da32f77af9805312944fe7a8b6d542490fc5876 11460 libsecret_0.21.4-1build3.debian.tar.xz
Checksums-Sha256:
 163d08d783be6d4ab9a979ceb5a4fecbc1d9660d3c34168c581301cd53912b20 206636 libsecret_0.21.4.orig.tar.xz
 0744496f6cd4849c6ed67ff2369dc09024d431382e09c4a0c24134ef464c6a94 11460 libsecret_0.21.4-1build3.debian.tar.xz
Files:
 85f0534c673ccf10daf6fa419728e0d8 206636 libsecret_0.21.4.orig.tar.xz
 99b9d6706e0d58a7e94cfee6a156df06 11460 libsecret_0.21.4-1build3.debian.tar.xz
Original-Maintainer: Debian GNOME Maintainers <pkg-gnome-maintainers@lists.alioth.debian.org>

-----BEGIN PGP SIGNATURE-----

iQJOBAEBCgA4FiEErEg/aN5yj0PyIC/KVo0w8yGyEz0FAmYIx24aHHN0ZXZlLmxh
bmdhc2VrQHVidW50dS5jb20ACgkQVo0w8yGyEz39Ag/8DLZh6jJlMgMUniba9yd4
GnAyCFbpNbGTFfKPDJ64yTV5w4VqcCSCAiG5GmCz4yZYP0MxprG7mGNsbgdA8z2r
DH0nAQzP5w2dgEYFRTDgiMsPtZI68/PJtHrZdlz5o0PGuzQxoRunjzyVpfXBB4xX
V6YcmGKc+GQR2HOILa68sRYGzfKzYF7Z5RAGMuh6HVxa+kAWEI4WNTZCp9c6TH2a
uZoAtI4eYjILejlWVXjFpfXZStsgJ43UFgpTS1YqCv/e9pfAqd+BREzewNaQpfiM
MobjrDxPsoyPwk34zKDDpCJK10nHlvco6Gi/eR9BnetYTJLs3rdVe/7OFmkkJbAW
lzLhekb5+fTse2LjM9kuK9phNotF0oiFUw/7mpcLtcn6BrwGOaNXx3jZ3ZAjBqkZ
LBhLU+E7nURcxSxGrFKr3R/jYYEv16XZKWlKFnmybiakELUPCPs+3G7FSWaq30fV
disQCXVHKmg9FceE2kMimi/d+OZ4r2cz+t0w6drrxgfWnUaHJHmGjWMGmY9pSXB2
+BDBSmT+TkyX3sDw9MBh507tDiyvVx8T6sj95O9NEUSHsx0tnNB+fkkqaFDbU6HJ
tpj+s5CEDBNkz2UaUf9Him7PtoT0SObZXQga7J72mOlj+M+B11HAgXw0dw72FKN7
V90g8ntyD3aOnr0rRPb8gVA=
=lGMN
-----END PGP SIGNATURE-----
