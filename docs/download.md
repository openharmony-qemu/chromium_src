# 代码下载命令

 repo init -u https://gitcode.com/openharmony-tpc/manifest.git -b 132_trunk --no-repo-verify

# 编译形态

oh-chromium_132_trunk
oh-chromium-rk64_132_trunk

# 具体编译命令

oh-chromium_132_trunk

precompile：

yes y | apt-get install libstdc++-10-dev libnss3-dev libnss3;npm config set registry https://repo.huaweicloud.com/repository/npm/; npm config set @ohos:registry https://repo.harmonyos.com/npm/; npm config set strict-ssl false

compileCMD：

echo 'start' &&export CCACHE_BASE="${PWD}" && export NO_DEVTOOL=1 && export CCACHE_LOCAL_DIR=.ccache_xts && export ZIP_COMPRESS_LEVEL=1 && export CCACHE_NOHASHDIR="true"&& export LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libstdc++.so.6:$LD_PRELOAD && export CCACHE_SLOPPINESS="include_file_ctime"&&ln -snf /usr/bin/python2.7 /usr/local/bin/python && bash build_arkweb.sh rk3568 -t w -A&& ./sign.sh rk3568&& ln -snf /usr/bin/python3.8 /usr/local/bin/python

resultlist：
src/out/rk3568/lib.unstripped/libarkweb_engine.so src/out/rk3568/lib.unstripped/libarkweb_render.so  src/out/rk3568/lib.unstripped/libarkweb_crashpad_handler.so src/out/rk3568/NWeb-rk3568.hap

oh-chromium-rk64_132_trunk：

precompile：
yes y | apt-get install libstdc++-10-dev libnss3-dev libnss3;npm config set registry https://repo.huaweicloud.com/repository/npm/; npm config set @ohos:registry https://repo.harmonyos.com/npm/; npm config set strict-ssl false

compileCMD：
echo 'start' &&export CCACHE_BASE="${PWD}" && export NO_DEVTOOL=1 && export CCACHE_LOCAL_DIR=.ccache_xts && export ZIP_COMPRESS_LEVEL=1 && export CCACHE_NOHASHDIR="true"&& export LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libstdc++.so.6:$LD_PRELOAD && export CCACHE_SLOPPINESS="include_file_ctime"&&ln -snf /usr/bin/python2.7 /usr/local/bin/python &&bash build_arkweb.sh rk3568_64 -t w -A&& ./sign.sh rk3568_64&& ln -snf /usr/bin/python3.8 /usr/local/bin/python

resultlist：
src/out/rk3568_64/lib.unstripped/libarkweb_engine.so src/out/rk3568_64/lib.unstripped/libarkweb_render.so src/out/rk3568_64/NWeb-rk3568_64.hap src/out/rk3568_64/lib.unstripped/libarkweb_crashpad_handler.so 