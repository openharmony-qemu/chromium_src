#!/bin/bash
# Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of
#    conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list
#    of conditions and the following disclaimer in the documentation and/or other materials
#    provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may be used
#    to endorse or promote products derived from this software without specific prior written
#    permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# use ./build.sh -nosym to build content_shell without symbol.
set -e
basedir=$(dirname "$0")
CUR_DIR=$PWD
ROOT_DIR="${CUR_DIR%/src*}""/src"
# Global variables.
BUILD_TARGET_CONTENT_SHELL="content_shell_web"
BUILD_TARGET_CHROME="chrome_main_web"
TEXT_BOLD="\033[1m"
TEXT_NORMAL="\033[0m"

#Add build args begin
buildargs="
  target_os=\"ohos\"
  is_component_build=false
  is_chrome_branded=false
  use_official_google_api_keys=false
  use_ozone=true
  use_aura=true
  use_v8_context_snapshot=true
  ozone_auto_platforms=false
  ozone_platform=\"ohos\"
  ozone_platform_ohos=true
  enable_extensions=true
  ffmpeg_branding=\"Chrome\"
  use_kerberos=false
  use_bundled_fontconfig=true
  enable_resource_allowlist_generation=false
  clang_use_chrome_plugins=false
  enable_message_center=true
  use_custom_libcxx=false
  use_sysroot=true
  gpu_switch=\"on\"
  proprietary_codecs=true
  media_use_ffmpeg=true
  media_use_openh264=true
  angle_enable_vulkan_system_info=true
  v8_enable_builtins_optimization=false
  enable_rust=true
  enable_rust_cxx=true
  enable_chromium_prelude=true
  clang_base_path=\"//ohos_sdk/openharmony/native/llvm\"
  llvm_ohos_mainline=true
  rustc_version=\"bca5fdebe0e539d123f33df5f2149d5976392e76-1-llvmorg-20-init-9764-gb81d8e90\"
  rust_sysroot_absolute=\"//ohos_sdk/rust-toolchain\"
  "
#Add build args end

#build args: build mode
is_debug=false
is_official_build=true
#build args: build mode end

buildgn=1
buildcount=0
buildccache=0
buildsymbol=0
buildproduct=""
buildarg_cpu="target_cpu=\"arm64\""
buildarg_musl="use_musl=true"
build_dir="out/musl_64/"
build_product_name="product_name=\"all\""
build_target="${BUILD_TARGET_CHROME}"
build_output=""
build_asan=0

usage() {
  echo -ne "USAGE: $0 [OPTIONS] [PRODUCT]

${TEXT_BOLD}OPTIONS${TEXT_NORMAL}:
  -j N              force number of build jobs
  -ccache           Enable CCache.
  -t <target>       Build target, for example::
                      ./build.sh
                      ./build.sh -t \"base:base\"
                      ./build.sh -t \"content/test:content_unittests\"
  -o <output_dir>   Output directory, for example: Default.
  -d                Build with Debug mode.

"
}

while [ "$1" != "" ]; do
  case $1 in
    "-j")
      shift
      buildcount=$1
    ;;
    "-ccache")
      buildccache=1
    ;;
    "-sym")
      buildsymbol=1
    ;;
    "-t")
      shift
      build_target=$1
      ;;
    "-o")
      shift
      build_output=$1
      ;;
    "-d")
      is_debug=true
      is_official_build=false
      echo "build mode: Debug"
      ;;
    "-h")
      usage
      exit 0
      ;;
    *)
      echo " -> $1 <- is not a valid option, please follow the usage below: "
      usage
      exit 1
    ;;
  esac
  shift
done

buildargs="${buildargs} is_debug=${is_debug} is_official_build=${is_official_build} "

if [ "-${build_output}" != "-" ]; then
  build_dir="out/${build_output}/"
fi

case "${build_target}" in
  "c"|"${BUILD_TARGET_CONTENT_SHELL}")
    build_target="${BUILD_TARGET_CONTENT_SHELL}"
    buildargs="${buildargs}safe_browsing_mode=0
    ohos_build_target=\"content_shell\"
    "
    ;;
  "b"|"${BUILD_TARGET_CHROME}")
    build_target="${BUILD_TARGET_CHROME}"
    buildargs="${buildargs}safe_browsing_mode=1
    ohos_build_target=\"chromium\"
    "
    ;;
  *)
    echo "build_target: ${build_target}"
    ;;
esac

SYMBOL_LEVEL=1
if [ $buildsymbol = 1 ]; then
  SYMBOL_LEVEL=2
fi

if [ $buildcount = 0 ]; then
  buildcount=$(grep processor /proc/cpuinfo | wc -l)
fi

if [ $buildccache = 1 ]; then
  if [ $buildcount = 0 ]; then
    buildcount=64
  fi
  GN_ARGS="cc_wrapper=\"ccache\" clang_use_chrome_plugins=false linux_use_bundled_binutils=false"
  export CCACHE_CPP2=yes
fi

if [ ${build_asan} -eq 1 ]; then
  GN_ARGS="${GN_ARGS} is_asan=true"
else
  GN_ARGS="${GN_ARGS} is_asan=false"
fi

# Extract ohos-sdk.
if [ -f "src/ohos_sdk/.install" ]; then
  bash "src/ohos_sdk/.install"
  if [ $? -ne 0 ]; then
    echo "ERROR: Failed to install ohos-sdk, abort!"
    exit 1
  fi
fi

cd src

time_start_for_build=$(date +%s)
time_start_for_gn=$time_start_for_build

if [ $buildgn = 1 ]; then
  echo "generating args list: $buildargs $GN_ARGS"
  third_party/depot_tools/gn gen $build_dir --args="$buildargs $buildarg_cpu $buildarg_musl $build_product_name $GN_ARGS symbol_level=$SYMBOL_LEVEL"
fi
time_end_for_gn=$(date +%s)

third_party/depot_tools/ninja -C $build_dir -j$buildcount ${build_target}

# generate compile_commands.json
third_party/depot_tools/ninja -C $build_dir -t compdb > "${build_dir}compile_commands.json"

time_end_for_build=$(date +%s)

time_format() {
  hours=$((($1 - $2) / 3600))
  minutes=$((($1 - $2) % 3600 / 60))
  seconds=$((($1 - $2) % 60))
}

time_format $time_end_for_gn $time_start_for_gn
printf "\n\e[32mTime for gn  : %dH:%dM:%dS \e[0m\n" $hours $minutes $seconds
time_format $time_end_for_build $time_end_for_gn
printf "\e[32mTime for build : %dH:%dM:%dS \e[0m\n" $hours $minutes $seconds
time_format $time_end_for_build $time_start_for_build
printf "\e[32mTime for Total : %dH:%dM:%dS \e[0m\n\n" $hours $minutes $seconds

echo "build done"
