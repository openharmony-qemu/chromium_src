!host_build|!cross_compile {
    QMAKE_CFLAGS=-g -O2 -ffile-prefix-map=/build/reproducible-path/qtbase-opensource-src-5.15.2+dfsg=. -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
    QMAKE_CXXFLAGS=-g -O2 -ffile-prefix-map=/build/reproducible-path/qtbase-opensource-src-5.15.2+dfsg=. -fstack-protector-strong -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
    QMAKE_LFLAGS=-Wl,-z,relro -Wl,--as-needed
}
QT_CPU_FEATURES.i386 = 
QT.global_private.enabled_features = alloca_h alloca dbus dbus-linked dlopen gui libudev network posix_fallocate reduce_exports reduce_relocations release_tools sql system-zlib testlib widgets xml zstd
QT.global_private.disabled_features = sse2 alloca_malloc_h android-style-assets avx2 private_tests gc_binaries intelcet relocatable stack-protector-strong
PKG_CONFIG_EXECUTABLE = /usr/bin/i686-linux-gnu-pkg-config
QMAKE_LIBS_DBUS = -ldbus-1
QMAKE_INCDIR_DBUS = /usr/include/dbus-1.0 /usr/lib/i386-linux-gnu/dbus-1.0/include
QMAKE_LIBS_LIBDL = -ldl
QMAKE_LIBS_LIBUDEV = -ludev
QT_COORD_TYPE = double
QMAKE_LIBS_ZLIB = -lz
QMAKE_LIBS_ZSTD = -lzstd
CONFIG -= precompile_header
QT_BUILD_PARTS += libs examples tools
CONFIG += compile_examples enable_new_dtags largefile rdrnd rdseed nostrip x86SimdAlways
QT_HOST_CFLAGS_DBUS += -I/usr/include/dbus-1.0 -I/usr/lib/i386-linux-gnu/dbus-1.0/include
