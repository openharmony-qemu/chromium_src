# 代码下载命令

repo init -u https://gitcode.com/openharmony-tpc/manifest.git -b 114_trunk_5.1.0-Release --no-repo-verify

# 编译形态

oh-chromium-114_trunk_5.1.0
oh-chromium-rk64-114_trunk_5.1.0

# 具体编译命令

oh-chromium-114_trunk_5.1.0

precompile：

pwd;yes y | apt-get install libstdc++-10-dev

compileCMD：

echo 'start'&&pwd;export CCACHE_DIR="${PWD}/.ccache";export CCACHE_MAXSIZE=50G;export CCACHE_CPP2=true;export CCACHE_SLOPPINESS=time_macros;export CCACHE_BASEDIR=${PWD};CCACHE_DEBUG=1&&ln -snf /usr/bin/python2.7 /usr/local/bin/python&& ./prebuilts_download.sh && bash build.sh rk3568 -t w -A -ccache&& ./sign.sh rk3568 && ln -snf /usr/bin/python3.8 /usr/local/bin/python

resultlist：
src/out/rk3568/lib.unstripped/libarkweb_engine.so src/out/rk3568/lib.unstripped/libarkweb_render.so src/out/rk3568/ArkWebCore-rk3568.hap src/out/rk3568/lib.unstripped/libarkweb_crashpad_handler.so 

oh-chromium-rk64-114_trunk_5.1.0：

precompile：
pwd;yes y | apt-get install libstdc++-10-dev

compileCMD：
echo 'start' &&pwd ;export CCACHE_DIR="${PWD}/.ccache";export CCACHE_MAXSIZE=50G;export CCACHE_CPP2=true;export CCACHE_SLOPPINESS=time_macros;export CCACHE_BASEDIR=${PWD};CCACHE_DEBUG=1&&ln -snf /usr/bin/python2.7 /usr/local/bin/python && ./prebuilts_download.sh && bash build.sh rk3568_64 -t w -A -ccache&& ./sign.sh rk3568_64&& ln -snf /usr/bin/python3.8 /usr/local/bin/python 

resultlist：
src/out/rk3568_64/lib.unstripped/libarkweb_engine.so src/out/rk3568_64/lib.unstripped/libarkweb_render.so src/out/rk3568_64/ArkWebCore-rk3568_64.hap src/out/rk3568_64/lib.unstripped/libarkweb_crashpad_handler.so 