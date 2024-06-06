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

set -e

WORK_SPACE=$(cd $(dirname $0); pwd)

OHOS_GLUE_DIR=${WORK_SPACE}/ohos_glue
OHOS_GLUE_LOG_DIR=${1}
INTERFACE_DIR=${WORK_SPACE}/../deps_code/webview/ohos_interface

INTERFACE_INCLUDE_DIR=${INTERFACE_DIR}/include
INTERFACE_OHOS_GLUE_DIR=${INTERFACE_DIR}/ohos_glue

CLANG_FORMAT_DIR=${WORK_SPACE}/buildtools/linux64

handle_copy_dir() {
  local src_dir=$1
  local dst_dir=$2

  local parent_dir=$(dirname ${dst_dir})
  [ -n "${dst_dir}" ] && rm -rf ${dst_dir} && mkdir -p ${parent_dir}

  if [ -d ${src_dir} ] && [ "$(ls -A ${src_dir})" ]; then
    cp -rf ${src_dir} ${dst_dir}
  else
    echo "${src_dir} is not exist or empty" >> ${OHOS_GLUE_LOG_DIR}/prepare.log
  fi
}

handle_copy_files() {
  echo "begin to copy ohos glue files,module name is $1" >> ${OHOS_GLUE_LOG_DIR}/prepare.log

  if [ "$1" = "base" ]; then
    mkdir -p ${OHOS_GLUE_DIR}
    cp ${INTERFACE_OHOS_GLUE_DIR}/BUILD_webcore.gn ${OHOS_GLUE_DIR}/BUILD.gn

    handle_copy_dir ${INTERFACE_OHOS_GLUE_DIR}/base ${OHOS_GLUE_DIR}/base
    handle_copy_dir ${INTERFACE_OHOS_GLUE_DIR}/scripts ${OHOS_GLUE_DIR}/scripts
    return
  fi

  local dir_name=ohos_$1
  rm -rf ${OHOS_GLUE_DIR}/${dir_name} && mkdir -p ${OHOS_GLUE_DIR}/${dir_name}
  handle_copy_dir ${INTERFACE_OHOS_GLUE_DIR}/${dir_name}/include ${OHOS_GLUE_DIR}/${dir_name}/include
  handle_copy_dir ${INTERFACE_OHOS_GLUE_DIR}/${dir_name}/bridge/webcore ${OHOS_GLUE_DIR}/${dir_name}/bridge
  handle_copy_dir ${INTERFACE_OHOS_GLUE_DIR}/${dir_name}/cpptoc/webcore ${OHOS_GLUE_DIR}/${dir_name}/cpptoc
  handle_copy_dir ${INTERFACE_OHOS_GLUE_DIR}/${dir_name}/ctocpp/webcore ${OHOS_GLUE_DIR}/${dir_name}/ctocpp
}

handle_copy_include() {
  echo "begin to copy ohos interface files" >> ${OHOS_GLUE_LOG_DIR}/prepare.log

  cp ${INTERFACE_INCLUDE_DIR}/ohos_nweb/* ${WORK_SPACE}/ohos_nweb/include
  cp ${INTERFACE_INCLUDE_DIR}/ohos_adapter/* ${WORK_SPACE}/third_party/ohos_ndk/includes/ohos_adapter
}

handle_copy_commond() {
  local curr_time=$(date +"%Y-%m-%d %H:%M:%S")
  echo "start time is ${curr_time}" >> ${OHOS_GLUE_LOG_DIR}/prepare.log
  echo "work space is ${WORK_SPACE}" >> ${OHOS_GLUE_LOG_DIR}/prepare.log
  echo "ohos glue dir is ${OHOS_GLUE_DIR}" >> ${OHOS_GLUE_LOG_DIR}/prepare.log

  handle_copy_include

  handle_copy_files base
  handle_copy_files nweb
  handle_copy_files adapter
}

handle_translate_commond() {
  OHOS_GLUE_MODULE=$1
  if [ "${OHOS_GLUE_MODULE}" = "base" ]; then
    return
  fi

  local curr_time=$(date +"%Y-%m-%d %H:%M:%S")
  echo "start time is ${curr_time}" >> ${OHOS_GLUE_LOG_DIR}/prepare.log
  echo "begin to translate ohos glue file,module name is ${OHOS_GLUE_MODULE}" >> ${OHOS_GLUE_LOG_DIR}/prepare.log

  local dir_name=ohos_${OHOS_GLUE_MODULE}
  local file_list=$(find ${OHOS_GLUE_DIR}/${dir_name}/include -name "*.h")
  for file in $file_list
  do
    ${CLANG_FORMAT_DIR}/clang-format -style="{PointerAlignment: Right}" -i $file
  done

  python3 ${OHOS_GLUE_DIR}/scripts/translator.py webcore $dir_name >> ${OHOS_GLUE_LOG_DIR}/prepare.log

  cp ${WORK_SPACE}/.clang-format ${OHOS_GLUE_DIR}
  file_list=$(find ${OHOS_GLUE_DIR}/${dir_name} -type f \( -name "*.h" -o -name "*.cpp" \))
  for file in $file_list
  do
    ${CLANG_FORMAT_DIR}/clang-format -style=file -i $file
  done
}

handle_copy_commond
handle_translate_commond nweb
handle_translate_commond adapter
