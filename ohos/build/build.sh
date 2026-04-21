#!/bin/bash
# Copyright (c) 2024 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

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
  enable_cem_update_devconfig=false
  clang_base_path=\"//ohos_sdk/openharmony/native/llvm\"
  llvm_ohos_mainline=true
  rustc_version=\"bca5fdebe0e539d123f33df5f2149d5976392e76-1-llvmorg-20-init-9764-gb81d8e90\"
  rust_sysroot_absolute=\"//ohos_sdk/rust-toolchain\"
  enable_crashpad=true
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
build_isolated_level=0
build_gwp_asan=0
build_hwasan=0

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
  -asan             Enable AddressSanitizer (ASan).
  -d                Build with Debug mode.
  -isl              Support the render process to enable sandbox isolation.
  -gwp_asan         Enable GWP-ASan.
  -hwasan           Enable Hardware Address Sanitizer (HWASan).
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
    "-asan")
      build_asan=1
      ;;
    "-hwasan")
      build_hwasan=1
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
    "-isl")
      build_isolated_level=1
      ;;
    "-gwp_asan")
      build_gwp_asan=1
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

if [ ${build_hwasan} -eq 1 ]; then
  GN_ARGS="${GN_ARGS}
    is_hwasan=true
    use_thin_lto=false
    v8_enable_pointer_compression = false
    v8_use_external_startup_data = false
    enable_native_child_process = false"
else
  GN_ARGS="${GN_ARGS} is_hwasan=false"
fi

if [ ${build_isolated_level} -eq 1 ]; then
  isolated_level=1
  buildargs="${buildargs} isolated_level=$isolated_level"
fi

if [ ${build_gwp_asan} -eq 1 ]; then
  GN_ARGS="${GN_ARGS} gwp_asan_enabled=true"
else
  GN_ARGS="${GN_ARGS} gwp_asan_enabled=false"
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
echo "generate compile db: ${build_dir}compile_commands.json in the background"

# generate compile_commands.json in the background, Save time.
(
tools/clang/scripts/generate_compdb.py -p $build_dir > "${build_dir}compile_commands.json.bak"
mv "${build_dir}compile_commands.json.bak" "${build_dir}compile_commands.json"
)&

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
