-----BEGIN PGP SIGNED MESSAGE-----
Hash: SHA256

Format: 3.0 (quilt)
Source: pulseaudio
Binary: pulseaudio, pulseaudio-utils, pulseaudio-module-zeroconf, pulseaudio-module-jack, pulseaudio-module-lirc, pulseaudio-module-gsettings, pulseaudio-module-raop, pulseaudio-module-bluetooth, pulseaudio-equalizer, libpulse0, libpulse-mainloop-glib0, libpulse-dev
Architecture: any
Version: 1:16.1+dfsg1-2ubuntu10
Maintainer: Ubuntu Developers <ubuntu-devel-discuss@lists.ubuntu.com>
Uploaders: Sjoerd Simons <sjoerd@debian.org>, Felipe Sateler <fsateler@debian.org>, Sebastien Bacher <seb128@debian.org>
Homepage: https://www.pulseaudio.org
Standards-Version: 4.6.0
Vcs-Browser: https://salsa.debian.org/pulseaudio-team/pulseaudio
Vcs-Git: https://salsa.debian.org/pulseaudio-team/pulseaudio.git
Testsuite: autopkgtest
Testsuite-Triggers: build-essential
Build-Depends: debhelper-compat (= 13), meson, ninja-build, check <!nocheck>, desktop-file-utils <!nocheck>, dh-exec, doxygen, dpkg-dev (>= 1.17.14), intltool, libasound2-dev [linux-any], libapparmor-dev [linux-any], libasyncns-dev, libavahi-client-dev, libbluetooth-dev [linux-any] <!stage1>, libsbc-dev [linux-any], libcap-dev [linux-any], libfftw3-dev, libglib2.0-dev, libgstreamer1.0-dev (>= 1.14), libgstreamer-plugins-base1.0-dev, libgtk-3-dev, libice-dev, libjack-dev, liblirc-dev, libltdl-dev, liborc-0.4-dev, libsnapd-glib-dev, libsndfile1-dev, libsoxr-dev (>= 0.1.1), libspeexdsp-dev (>= 1.2~rc1), libssl-dev, libsystemd-dev [linux-any], libtdb-dev, libudev-dev [linux-any], libwebrtc-audio-processing-dev (>= 0.2) [linux-any], libwrap0-dev, libx11-xcb-dev, libxcb1-dev, libxml2-utils <!nocheck>, libxtst-dev, systemd [linux-any]
Package-List:
 libpulse-dev deb libdevel optional arch=any
 libpulse-mainloop-glib0 deb libs optional arch=any
 libpulse0 deb libs optional arch=any
 pulseaudio deb sound optional arch=any
 pulseaudio-equalizer deb sound optional arch=any
 pulseaudio-module-bluetooth deb sound optional arch=linux-any profile=!stage1
 pulseaudio-module-gsettings deb sound optional arch=any
 pulseaudio-module-jack deb sound optional arch=any
 pulseaudio-module-lirc deb sound optional arch=any
 pulseaudio-module-raop deb sound optional arch=any
 pulseaudio-module-zeroconf deb sound optional arch=any
 pulseaudio-utils deb sound optional arch=any
Checksums-Sha1:
 921990eedeafd42278ae6ca71487ebdf74b340e0 1437216 pulseaudio_16.1+dfsg1.orig.tar.xz
 b2d342718f1f9a89f3c63410abb2d33b16a86dca 97852 pulseaudio_16.1+dfsg1-2ubuntu10.debian.tar.xz
Checksums-Sha256:
 e471c61c47b6d95089be52a5122c87161a695b60534a0c8a36fe344310fd3d06 1437216 pulseaudio_16.1+dfsg1.orig.tar.xz
 273a72b28b0a2248a772ebddab9c17a2d71fe77058fc31d960427e1c063bf66f 97852 pulseaudio_16.1+dfsg1-2ubuntu10.debian.tar.xz
Files:
 436f9a32a0d1588f4ac956b97c6282ea 1437216 pulseaudio_16.1+dfsg1.orig.tar.xz
 5a97d216c70750dad6c7f7de1611f638 97852 pulseaudio_16.1+dfsg1-2ubuntu10.debian.tar.xz
Original-Maintainer: Pulseaudio maintenance team <pkg-pulseaudio-devel@lists.alioth.debian.org>

-----BEGIN PGP SIGNATURE-----

iQIzBAEBCAAdFiEEkv5Pi0DK3E7M1gwfWWH9vpxhmAkFAmYNERQACgkQWWH9vpxh
mAmyXRAAh9y8CHMYkRiwIrCGh2q/s4UmaIfzHRXFt18Nm8aHMEipPQ27XJeDqRt4
FDK0KefKhpZn6CaCKNYc9p9CefMycY3JmwcIyzWrElUoRhoSAR9ldHoTiy3536TC
ciVsin3Xtyh5xfxEvMBfxJoBPYSL315P0DmlI9cUwbw43MJ+zTaJ5f2j2uuaHlG1
4f1A6SxYXh+e/yDVD315Eb6A6kQjvtBpKa/hgDLP3/gQmSSh6fSrmsmr2VhmDrvJ
L9NPEuAKyPU037CVWW4ErBqyBPLGp4T+57C7mvvDPLp9JnCMP4gsauGoqBbp74zY
edLaPgjTB4sS5DNLwnSknQ+dpCXlF8VC6mrI/TpvbVBnGo+Y+oWi4KEgZC/BnAXL
mrhckpp2RF9pdRIG4IXk28GYA+APBu/6Z0VsqyEAE3UxuMXP4tyVucHrR51Or0pi
wBr/13vVhlKQRNAnUcDiy4Iop2ItslWB3BXQYeAlKCvNXDXycGBwLjqeOVPF8flm
drhOHdZ68eMjdHJFrdMHZ7hkulEa3BQUGyK/YVcSqr9Qgb+4wAcLK4sFBgGIDVE/
MXiBnqgWcEWkMGQ5lz4l5HBLsCGF0F4nMcv8OZaa3fWR9X+0OAOFOvS9r47Vu8Bh
Eqr36QkxWbs+zBnZ9+v2LiH22Y6LUui0Sj1cGzPzEmyEtUFq6Y4=
=Ab7g
-----END PGP SIGNATURE-----
