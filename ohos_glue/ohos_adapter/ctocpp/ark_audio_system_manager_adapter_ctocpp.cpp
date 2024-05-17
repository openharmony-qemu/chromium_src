/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_adapter/ctocpp/ark_audio_system_manager_adapter_ctocpp.h"
#include "base/ctocpp/ark_web_ctocpp_macros.h"
#include "ohos_adapter/cpptoc/ark_audio_interrupt_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_audio_manager_callback_adapter_cpptoc.h"
#include "ohos_adapter/cpptoc/ark_audio_manager_device_change_callback_adapter_cpptoc.h"
#include "ohos_adapter/ctocpp/ark_audio_device_desc_adapter_ctocpp.h"

namespace OHOS::ArkWeb {

ARK_WEB_NO_SANITIZE
bool ArkAudioSystemManagerAdapterCToCpp::HasAudioOutputDevices() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_system_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, has_audio_output_devices, false);

  // Execute
  return _struct->has_audio_output_devices(_struct);
}

ARK_WEB_NO_SANITIZE
bool ArkAudioSystemManagerAdapterCToCpp::HasAudioInputDevices() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_system_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, false);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, has_audio_input_devices, false);

  // Execute
  return _struct->has_audio_input_devices(_struct);
}

ARK_WEB_NO_SANITIZE
int32_t ArkAudioSystemManagerAdapterCToCpp::RequestAudioFocus(
    const ArkWebRefPtr<ArkAudioInterruptAdapter> audioInterrupt) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_system_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, request_audio_focus, 0);

  // Execute
  return _struct->request_audio_focus(
      _struct, ArkAudioInterruptAdapterCppToC::Invert(audioInterrupt));
}

ARK_WEB_NO_SANITIZE
int32_t ArkAudioSystemManagerAdapterCToCpp::AbandonAudioFocus(
    const ArkWebRefPtr<ArkAudioInterruptAdapter> audioInterrupt) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_system_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, abandon_audio_focus, 0);

  // Execute
  return _struct->abandon_audio_focus(
      _struct, ArkAudioInterruptAdapterCppToC::Invert(audioInterrupt));
}

ARK_WEB_NO_SANITIZE
int32_t ArkAudioSystemManagerAdapterCToCpp::SetAudioManagerInterruptCallback(
    ArkWebRefPtr<ArkAudioManagerCallbackAdapter> callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_system_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct,
                                   set_audio_manager_interrupt_callback, 0);

  // Execute
  return _struct->set_audio_manager_interrupt_callback(
      _struct, ArkAudioManagerCallbackAdapterCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
int32_t
ArkAudioSystemManagerAdapterCToCpp::UnsetAudioManagerInterruptCallback() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_system_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct,
                                   unset_audio_manager_interrupt_callback, 0);

  // Execute
  return _struct->unset_audio_manager_interrupt_callback(_struct);
}

ARK_WEB_NO_SANITIZE
ArkAudioDeviceDescAdapterVector
ArkAudioSystemManagerAdapterCToCpp::GetDevices(int32_t flag) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_system_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, {0});

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_devices, {0});

  // Execute
  return _struct->get_devices(_struct, flag);
}

ARK_WEB_NO_SANITIZE
int32_t
ArkAudioSystemManagerAdapterCToCpp::SelectAudioDeviceById(int32_t deviceId,
                                                          bool isInput) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_system_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, select_audio_device_by_id, 0);

  // Execute
  return _struct->select_audio_device_by_id(_struct, deviceId, isInput);
}

ARK_WEB_NO_SANITIZE
int32_t ArkAudioSystemManagerAdapterCToCpp::SetDeviceChangeCallback(
    ArkWebRefPtr<ArkAudioManagerDeviceChangeCallbackAdapter> callback) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_system_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_device_change_callback, 0);

  // Execute
  return _struct->set_device_change_callback(
      _struct,
      ArkAudioManagerDeviceChangeCallbackAdapterCppToC::Invert(callback));
}

ARK_WEB_NO_SANITIZE
int32_t ArkAudioSystemManagerAdapterCToCpp::UnsetDeviceChangeCallback() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_system_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, unset_device_change_callback, 0);

  // Execute
  return _struct->unset_device_change_callback(_struct);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkAudioDeviceDescAdapter>
ArkAudioSystemManagerAdapterCToCpp::GetDefaultOutputDevice() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_system_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_default_output_device, nullptr);

  // Execute
  ark_audio_device_desc_adapter_t *_retval =
      _struct->get_default_output_device(_struct);

  // Return type: refptr_same
  return ArkAudioDeviceDescAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
ArkWebRefPtr<ArkAudioDeviceDescAdapter>
ArkAudioSystemManagerAdapterCToCpp::GetDefaultInputDevice() {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_system_manager_adapter_t *_struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, nullptr);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, get_default_input_device, nullptr);

  // Execute
  ark_audio_device_desc_adapter_t *_retval =
      _struct->get_default_input_device(_struct);

  // Return type: refptr_same
  return ArkAudioDeviceDescAdapterCToCpp::Invert(_retval);
}

ARK_WEB_NO_SANITIZE
bool ArkAudioSystemManagerAdapterCToCpp::SetLanguage(
    ArkWebString language) {
  ARK_WEB_CTOCPP_DV_LOG("capi struct is %{public}ld", (long)this);

  ark_audio_system_manager_adapter_t* _struct = GetStruct();
  ARK_WEB_CTOCPP_CHECK_PARAM(_struct, 0);

  ARK_WEB_CTOCPP_CHECK_FUNC_MEMBER(_struct, set_language, 0);

  // Execute
  return _struct->set_language(_struct, language);
}

ArkAudioSystemManagerAdapterCToCpp::ArkAudioSystemManagerAdapterCToCpp() {
}

ArkAudioSystemManagerAdapterCToCpp::~ArkAudioSystemManagerAdapterCToCpp() {
}

template <>
ArkWebBridgeType ArkWebCToCppRefCounted<
    ArkAudioSystemManagerAdapterCToCpp, ArkAudioSystemManagerAdapter,
    ark_audio_system_manager_adapter_t>::kBridgeType =
    ARK_AUDIO_SYSTEM_MANAGER_ADAPTER;

} // namespace OHOS::ArkWeb
