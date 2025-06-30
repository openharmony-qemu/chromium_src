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

base_dir="$(dirname "$(realpath "${BASH_SOURCE[0]}")")"
WORK_SPACE=${base_dir}/../../
cd ${WORK_SPACE}
echo "start prepare, workspace: ${WORK_SPACE}"

OHOS_GLUE_DIR=${WORK_SPACE}/ohos_glue
OHOS_GLUE_REVISION=${WORK_SPACE}/ohos_glue/Revision.txt
OHOS_GLUE_LOG_DIR=${1}
INTERFACE_DIR=${WORK_SPACE}/../deps_code/webview/ohos_interface

INTERFACE_INCLUDE_DIR=${INTERFACE_DIR}/include
INTERFACE_OHOS_GLUE_DIR=${INTERFACE_DIR}/ohos_glue

CLANG_FORMAT_DIR=${WORK_SPACE}/buildtools/linux64

NEED_PROCESS=1
NEW_REVISION=

need_processed() {
  if [ ! -d ${INTERFACE_DIR} ]; then
    NEED_PROCESS=0
    return
  fi

  cd ${INTERFACE_DIR}
  deps_code_revision=`git rev-parse HEAD`
  NEW_REVISION=$deps_code_revision
  cd -

  if [ ! -f ${OHOS_GLUE_REVISION} ]; then
    NEED_PROCESS=1
    return
  fi

  local_revision=`head -n 1 ${OHOS_GLUE_REVISION}`
  if [ "X$deps_code_revision" == "X$local_revision" ]; then
    echo "deps_code/webview's revision not changed, no need prepare ohos_glue."
    NEED_PROCESS=0
  else
    echo "deps_code/webview's revision: $deps_code_revision"
    echo "              local revision: $local_revision"
    echo "revision changed, re-prepare ohos_glue..."
    NEED_PROCESS=1
  fi
}

update_local_revision() {
  if [ "X$NEW_REVISION" != "X" ]; then
    echo "write deps_code/webview's revision into ${OHOS_GLUE_REVISION}:${NEW_REVISION}"
    echo "${NEW_REVISION}" > ${OHOS_GLUE_REVISION}
  fi
}

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
    # 胶水层后续完全删除，对ohos_adapter的调用将改为调用adapter_ndk。当前为暂时兼容方案，ohos_base_glue_source依赖adapter_ndk
    sed -i ':a;N;$!ba;s#"//ohos_glue:ohos_base_glue_source"#"//arkweb/ohos_adapter_ndk:adapter_ndk", "//ohos_glue:ohos_base_glue_source"#2' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已经依赖adapter_ndk且adapter_ndk已实现imfadapter，胶水层ark_imfadapter_wrapper.cpp不参与编译
    # sed -i '/ark_imfadapter_wrapper/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已经依赖adapter_ndk且adapter_ndk已实现ohos_adapter_helper，胶水层ark_imfadapter_wrapper.cpp不参与编译
    sed -i '/ark_ohos_adapter_helper_wrapper/{d}' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现media_codec_decoder_adapter，胶水层ark_media_codec_decoder_adapter_wrapper.cpp不参与编译
    sed -i '/ark_media_codec_decoder_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现audio_codec_decoder_adapter，胶水层ark_audio_codec_decoder_adapter_wrapper.cpp不参与编译
    sed -i '/ark_audio_codec_decoder_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现drm_adapter，胶水层ark_drm_adapter_wrapper.cpp不参与编译
    sed -i '/ark_drm_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现audio_cenc_info_adapter，胶水层ark_audio_cenc_info_adapter_wrapper.cpp不参与编译
    sed -i '/ark_audio_cenc_info_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已经依赖adapter_ndk且adapter_ndk已实现media_avsession_adapter，胶水层不参与编译
    sed -i '/ark_media_avsession_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已经依赖adapter_ndk且adapter_ndk已实现hilog，胶水层ark_hilog_adapter_wrapper.cpp不参与编译
    #sed -i '/ark_hilog_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn
    sed -i '/ark_native_image_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn
    sed -i '/ark_ohos_buffer_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn
    # 由于已经依赖adapter_ndk且adapter_ndk已实现hilog，胶水层ark_cert_mgr_adapter_impl_wrapper.cpp不参与编译
    sed -i '/ark_cert_mgr_adapter_impl_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现event_handler_adapter，胶水层ark_event_handler_adapter_wrapper.cpp不参与编译
    sed -i '/ark_event_handler_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn
    #由于已经依赖adapter_ndk且adapter_ndk已实现datashare，胶水层ark_datashare_adapter_wrapper.cpp不参与编译
    sed -i '/ark_datashare_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn
    # 由于已经依赖adapter_ndk且adapter_ndk已实现pasteboard，相关胶水层不参与编译
    sed -i '/ark_paste_board_client_adapter_wrapper/{d}' ${OHOS_GLUE_DIR}/BUILD.gn
    sed -i '/ark_paste_data_adapter_wrapper/{d}' ${OHOS_GLUE_DIR}/BUILD.gn
    sed -i '/ark_paste_data_record_adapter_wrapper/{d}' ${OHOS_GLUE_DIR}/BUILD.gn
    #由于已经依赖adapter_ndk且adapter_ndk已实现display manager，胶水层ark_display_manager_adapter_wrapper.cpp不参与编译
    #sed -i '/ark_display_manager_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已经依赖adapter_ndk且adapter_ndk已实现camera_adapter，相关胶水层文件不参与编译
    # sed -i '/ark_camera_manager_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn
    # sed -i '/ark_camera_rotation_info_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn
    # sed -i '/ark_format_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn
    # sed -i '/ark_video_capture_range_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn
    # sed -i '/ark_video_control_support_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn
    # sed -i '/ark_video_device_descriptor_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已经依赖adapter_ndk且adapter_ndk已实现audio_capturer_adapter，胶水层ark_audio_capturer_adapter_wrapper.cpp不参与编译
    sed -i '/ark_audio_capturer_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已经依赖adapter_ndk且adapter_ndk已实现screen_capturer_adapter，胶水层ark_screen_capturer_adapter_wrapper.cpp不参与编译
    sed -i '/ark_screen_capturer_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已经依赖adapter_ndk且adapter_ndk已实现audio_capturer_adapter，胶水层ark_audio_system_manager_adapter_wrapper.cpp不参与编译
    sed -i '/ark_audio_system_manager_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已经依赖adapter_ndk且adapter_ndk已实现audio_capturer_adapter，胶水层ark_audio_device_desc_adapter_wrapper.cpp不参与编译
    sed -i '/ark_audio_device_desc_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    #由于已经依赖adapter_ndk且adapter_ndk已实现dataBase，胶水层ark_ohos_web_data_base_adapter_wrapper不参与编译
    # sed -i '/ark_ohos_web_data_base_adapter_wrapper/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已经依赖adapter_ndk且adapter_ndk已实现player_adapter，胶水层ark_player_adapter_wrapper.cpp不参与编译
    sed -i '/ark_player_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn
    # 由于已经依赖adapter_ndk且adapter_ndk已实现iconsumer_surface_adapter.cpp，胶水层ark_iconsumer_surface_adapter_wrapper.cpp不参与编译
    sed -i '/ark_iconsumer_surface_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    #由于已经依赖adapter_ndk且adapter_ndk已实现dataBase，胶水层ark_ohos_access_token_adapter_wrapper不参与编译
    sed -i '/ark_access_token_adapter_impl_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn
    # 由于已经依赖adapter_ndk且adapter_ndk已实现ashmem，胶水层ark_ashmem_adapter_wrapper.cpp不参与编译
    sed -i '/ark_ashmem_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现print_manager_adapter，胶水层ark_print_manager_adapter_wrapper.cpp不参与编译
    # sed -i '/ark_net_proxy_adapter_impl_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现image_decoder_adapter，胶水层ark_ohos_image_decoder_adapter_wrapper.cpp不参与编译
    sed -i '/ark_ohos_image_decoder_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    #由于已经依赖adapter_ndk且adapter_ndk已实现dataBase，胶水层ark_ohos_net_connect_adapter_wrapper不参与编译
    # sed -i '/ark_net_connect_adapter_impl_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现system_properties_adapter，胶水层ark_system_properties_adapter_wrapper.cpp不参与编译
    # sed -i '/ark_system_properties_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现sensor_adapter，胶水层ark_sensor_adapter_wrapper.cpp不参与编译
    sed -i '/ark_sensor_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已经依赖adapter_ndk且adapter_ndk已实现media_codec相关接口，相关胶水层不参与编译
    sed -i '/ark_media_codec_list_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn
    sed -i '/ark_capability_data_adapter_impl.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现print_manager_adapter，胶水层ark_print_manager_adapter_wrapper.cpp不参与编译
    # sed -i '/ark_print_manager_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现BatteryMgrClientAdapter，胶水层ark_battery_mgr_client_adapter_wrapper.cpp不参与编译
    sed -i '/ark_battery_mgr_client_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn
    # 由于已经依赖adapter_ndk且adapter_ndk已实现ashmem，胶水层ark_keystore_adapter_impl_wrapper.cpp不参与编译
    sed -i '/ark_keystore_adapter_impl_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现DateTimeFormatAdapter，胶水层ark_date_time_format_adapter_wrapper.cpp不参与编译
    sed -i '/ark_date_time_format_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现VideoEncoderAdapter，胶水层ark_media_codec_encoder_adapter_wrapper.cpp不参与编译
    sed -i '/ark_media_codec_encoder_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现LocationInstance，胶水层ark_location_instance_wrapper.cpp不参与编译
    sed -i '/ark_location_instance_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现LocationProxyAdapter，胶水层ark_location_proxy_adapter_wrapper.cpp不参与编译
    sed -i '/ark_location_proxy_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现LocationRequestConfig，胶水层ark_location_request_config_wrapper.cpp不参与编译
    sed -i '/ark_location_request_config_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现HitraceAdapterWrapper，胶水层ark_hitrace_adapter_wrapper.cpp不参与编译
    sed -i '/ark_hitrace_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于已实现drawing_text_adapter，胶水层ark_ohos_drawing_text_adapter_wrapper.cpp不参与编译
    sed -i '/ark_ohos_drawing_text_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

    # 由于未使用，胶水层ark_power_mgr_client_adapter_wrapper.cpp不参与编译
    sed -i '/ark_power_mgr_client_adapter_wrapper.cpp/d' ${OHOS_GLUE_DIR}/BUILD.gn

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

handle_patch_apply() {
  #!/bin/sh
  CHROMIUM_DIR=${WORK_SPACE}/../
  #python3 cef/tools/patcher.py --root-dir=${CHROMIUM_DIR}
}

handle_patch_apply
need_processed
#临时遍历引用的头文件，添加#include <vector>
python3 ${WORK_SPACE}/arkweb/build/include_vector_patch.py ${INTERFACE_DIR}
if [ "$NEED_PROCESS" -eq 1 ]; then
  handle_copy_commond
  handle_translate_commond nweb
  handle_translate_commond adapter

  update_local_revision
fi
