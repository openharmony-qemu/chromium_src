#!/bin/bash
# Copyright (c) 2023 Huawei Device Co., Ltd.
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

SOURCE_ROOT_DIR=$(cd $(dirname $0);pwd)

WITH_OHPM=0
WITH_TOOLCHAIN=0
WITH_SYSROOT=0
WITH_SDK=0

if [ $# -eq 0 ]; then
  set -- "$@" "--all"
fi

while [ $# -gt 0 ]; do
  case "$1" in
    --with-ohpm)
      WITH_OHPM=1
    ;;
    --with-toolchain)
      WITH_TOOLCHAIN=1
    ;;
    --with-sysroot)
        WITH_SDK=1
        WITH_SYSROOT=1
    ;;
    --with-sdk)
      WITH_SDK=1
    ;;
    --all)
      WITH_OHPM=1; WITH_TOOLCHAIN=1; WITH_SYSROOT=1; WITH_SDK=1
    ;;
    --root-dir=*)
      SOURCE_ROOT_DIR=$(realpath "${1#--root-dir=}")
    ;;
    --tool-repo=*)
      TOOL_REPO="${1#--tool-repo=}"
    ;;
    *)
      echo "$0: Warning: unsupported parameter: $1" >&2
      exit 1
    ;;
  esac
  shift
done

PREBUILTS_ROOT_DIR=${SOURCE_ROOT_DIR}/prebuilts

OHPM_LINK_PATH=${SOURCE_ROOT_DIR}/src/huawei/third_party
TOOLCHAIN_LINK_PATH=${SOURCE_ROOT_DIR}/src/third_party/ohos_ndk/toolchains
SYSROOT_LINK_PATH=${SOURCE_ROOT_DIR}/src/third_party/ohos_ndk
SDK_LINK_PATH=${SOURCE_ROOT_DIR}/src/ohos_sdk
SDK_API_VERSION=12

function check_version() {
  installed_version=""
  version_file=$1
  latest_version=$2
  if [ -f "${version_file}" ]; then
    installed_version=$(head -n 1 "${version_file}")
  fi

  if [ "-${latest_version}" == "-${installed_version}" ]; then
    echo "[INFO] check version finished, already latest version. skip download"
    return 0
  fi
  return 1
}

function check_sha256() {
  check_sha256=$(curl -s -k $1.sha256)
  local_sha256=$(sha256sum $2 |cut -d ' ' -f1)
  if [ $check_sha256 == $local_sha256 ]; then
    return 0
  fi
  echo "[INFO] sha256 check not pass."
  return 1
}

function init_ohpm() {
  export PATH=${SOURCE_ROOT_DIR}/src/third_party/node/linux/node-linux-x64/bin:$PATH
  echo "[INFO] Current Node.js version is $(node -v)"
  npm config set registry https://repo.huaweicloud.com/repository/npm/
  npm config set @ohos:registry https://repo.harmonyos.com/npm/
  npm config set strict-ssl false
  npm config set lockfile false
  cat $HOME/.npmrc | grep 'lockfile=false' > /dev/null || echo 'lockfile=false' >> $HOME/.npmrc > /dev/null

  pushd ${PREBUILTS_ROOT_DIR} > /dev/null
    if [[ ! -f "${PREBUILTS_ROOT_DIR}/oh-command-line-tools/ohpm/bin/ohpm" ]]; then
      echo "[INFO] download oh-command-line-tools"
      file_name="ohcommandline-tools-linux.zip"
      wget https://contentcenter-vali-drcn.dbankcdn.cn/pvt_2/DeveloperAlliance_package_901_9/68/v3/r-5H8I7LT9mBjSFpSOY0Sg/ohcommandline-tools-linux-2.1.0.6.zip\?HW-CC-KV\=V1\&HW-CC-Date\=20231027T004601Z\&HW-CC-Expire\=315360000\&HW-CC-Sign\=A4D5E1A29C1C6962CA65592C3EB03ED363CE664CBE6C5974094064B67C34325E -O $file_name
      unzip -q $file_name
    fi
    OHPM_HOME=${PREBUILTS_ROOT_DIR}/oh-command-line-tools/ohpm
    export PATH=${OHPM_HOME}/bin:$PATH
    chmod +x ${OHPM_HOME}/bin/init
    ${OHPM_HOME}/bin/init > /dev/null
    echo "[INFO] Current ohpm version is $(ohpm -v)"
    ohpm config set registry https://repo.harmonyos.com/ohpm/
    ohpm config set strict_ssl false
    ohpm config set log_level debug
  popd > /dev/null
  if [[ -d "$HOME/.hvigor" ]]; then
    rm -rf $HOME/.hvigor/daemon $HOME/.hvigor/wrapper
  fi
  mkdir -p $HOME/.hvigor/wrapper/tools
  echo '{"dependencies": {"pnpm": "7.30.0"}}' > $HOME/.hvigor/wrapper/tools/package.json
  pushd $HOME/.hvigor/wrapper/tools > /dev/null
    echo "[INFO] installing pnpm..."
    npm install --silent > /dev/null
  popd > /dev/null
  mkdir -p $HOME/.ohpm
  echo '{"devDependencies":{"@ohos/hypium":"1.0.6"}}' > $HOME/.ohpm/oh-package.json5
  pushd $HOME/.ohpm > /dev/null
    echo "[INFO] installing hypium..."
    ohpm install > /dev/null
  popd > /dev/null

  if [[ -e "${OHPM_LINK_PATH}/ohpm" ]]; then
    rm -rf ${OHPM_LINK_PATH}/ohpm
  fi
  if [[ ! -d "${OHPM_LINK_PATH}" ]]; then
    mkdir -p ${OHPM_LINK_PATH}
  fi
  echo "[INFO] link ohpm"
  ln -s ${PREBUILTS_ROOT_DIR}/oh-command-line-tools/ohpm ${OHPM_LINK_PATH}/ohpm
}

function hwcloud_download() {
  if [ -z "${TOOL_REPO}" ]; then
    TOOL_REPO="https://mirrors.huaweicloud.com"
  fi

  download_url=${TOOL_REPO}/$1
  if [ -f $2 ] && (check_sha256 $download_url $2) ; then
    echo "[INFO] sha256 check pass. skip download, start decompressing..."
    return
  fi
  wget $download_url -O $2
  if [[ "$?" -ne 0 ]]; then
    echo -e "\033[31m[ERROR] wget download $2 failed!\033[0m"
    exit 1
  fi
}

function download_toolchain() {
  llvm_version="openharmony/compiler/clang/15.0.4-bf8f59/linux/clang_linux-x86_64-bf8f59-20240624.tar.gz"
  version_file=${PREBUILTS_ROOT_DIR}/llvm/.version

  if [[ ! -d "${PREBUILTS_ROOT_DIR}/llvm" ]] || ! (check_version $version_file $llvm_version); then
    file_name=$(basename $llvm_version)
    echo "[INFO] ready to download llvm toolchain"
    pushd ${PREBUILTS_ROOT_DIR} > /dev/null
      hwcloud_download $llvm_version $file_name
      tar -xf $file_name
      rm -rf llvm
      mv $(basename $llvm_version .tar.gz) llvm
    popd > /dev/null
    ln -s "15.0.4" ${PREBUILTS_ROOT_DIR}/llvm/lib/clang/current
    echo -n "${llvm_version}" > "${version_file}"
  fi
  if [[ -e "${TOOLCHAIN_LINK_PATH}/llvm" ]]; then
    rm -rf ${TOOLCHAIN_LINK_PATH}/llvm
  fi
  if [[ ! -d "${TOOLCHAIN_LINK_PATH}" ]]; then
    mkdir ${TOOLCHAIN_LINK_PATH}
  fi
  echo "[INFO] link llvm"
  ln -s ${PREBUILTS_ROOT_DIR}/llvm ${TOOLCHAIN_LINK_PATH}/llvm
}

function download_sdk() {
  sdk_item_list=("ets" "js" "native" "previewer" "toolchains")
  sdk_version="openharmony/os/5.0-Beta1/ohos-sdk-windows_linux-public.tar.gz"
  version_file=${PREBUILTS_ROOT_DIR}/ohos-sdk/.version

  if [[ ! -d "${PREBUILTS_ROOT_DIR}/ohos-sdk" ]] || ! (check_version $version_file $sdk_version); then
    file_name=$(basename $sdk_version)
    echo "[INFO] ready to download sdk"
    pushd ${PREBUILTS_ROOT_DIR} > /dev/null
      hwcloud_download $sdk_version $file_name
      rm -rf ohos-sdk
      mkdir ohos-sdk
      tar -xf $file_name -C ohos-sdk/
      pushd ohos-sdk/linux > /dev/null
        for i in ${sdk_item_list[@]}; do
          echo -ne "[INFO] Unzipping ${i}...\r"
          unzip -q ${i}-linux-x64-*-*.zip -d "$SDK_API_VERSION"
        done
        echo "[INFO] Unzipping finished.    "
        rm -f *.zip
      popd > /dev/null
    popd > /dev/null
    echo -n "${sdk_version}" > "${version_file}"
  fi
  if [[ -e "${SDK_LINK_PATH}/${SDK_API_VERSION}" ]]; then
   rm -rf ${SDK_LINK_PATH}/${SDK_API_VERSION}
  fi
  mkdir -p ${SDK_LINK_PATH}/${SDK_API_VERSION}
  echo "[INFO] link sdk"
  for i in ${sdk_item_list[@]}; do
    ln -s ${PREBUILTS_ROOT_DIR}/ohos-sdk/linux/$SDK_API_VERSION/${i} ${SDK_LINK_PATH}/${SDK_API_VERSION}/${i}
  done

  # For compatibility with src/ohos_sdk/.install
  echo -n "ohos-sdk-5.0.0.25-Beta1.tar.gz" > "${SDK_LINK_PATH}/.version"
}

function extract_sysroot() {
  version_file=$SYSROOT_LINK_PATH/sysroot/usr/.version
  file_path=$(ls ${SYSROOT_LINK_PATH}/sysroot-*.tar.gz 2>/dev/null)
  if [[ ! $file_path ]]; then
    echo "[WARNING] The sysroot tar file not found, skipped."
    return
  fi
  file_name=$(basename $file_path)
  if [[ ! -d "${SYSROOT_LINK_PATH}/sysroot/usr" ]] || ! (check_version $version_file $file_name); then
    echo "[INFO] extract ${file_name}"
    pushd ${SYSROOT_LINK_PATH} > /dev/null
      rm -rf sysroot
      tar -xzf ${file_name}
    popd > /dev/null
    echo -n "${file_name}" > "${version_file}"
  fi
}

function main() {
  if [[ ! -d "${SOURCE_ROOT_DIR}/src" ]]; then
    echo -e "\033[31m[ERROR] Please execute the prebuilts_download.sh from project root path\033[0m"
    exit 1
  fi

  if [[ ! -d "${PREBUILTS_ROOT_DIR}" ]]; then
    mkdir ${PREBUILTS_ROOT_DIR}
  fi

  if [[ ${WITH_OHPM} -eq 1 ]]; then
    echo "[INFO] Ohpm initialization started..."
    init_ohpm
    if [[ "$?" -ne 0 ]]; then
      echo -e "\033[31m[ERROR] Ohpm prebuilts download initialization failed!\033[0m"
      exit 1
    fi
  fi

  if [[ ${WITH_TOOLCHAIN} -eq 1 ]]; then
    echo "[INFO] Toolchain initialization started..."
    download_toolchain
    if [[ "$?" -ne 0 ]]; then
      echo -e "\033[31m[ERROR] Toolchain prebuilts download initialization failed!\033[0m"
      exit 1
    fi
  fi

  if [[ ${WITH_SDK} -eq 1 ]]; then
    echo "[INFO] Sdk initialization started..."
    download_sdk
    if [[ "$?" -ne 0 ]]; then
      echo -e "\033[31m[ERROR] Sdk prebuilts download initialization failed!\033[0m"
      exit 1
    fi
  fi

  if [[ ${WITH_SYSROOT} -eq 1 ]]; then
    echo "[INFO] Sysroot initialization started..."
    ln -s ${PREBUILTS_ROOT_DIR}/ohos-sdk/linux/${SDK_API_VERSION}/native/sysroot ${SYSROOT_LINK_PATH}/sysroot
    if [[ "$?" -ne 0 ]]; then
      echo -e "\033[31m[ERROR] Sysroot initialization failed!\033[0m"
      exit 1
    fi
  fi

  echo -e "\033[32m[INFO] prebuilts download completed\033[0m"
}

main
