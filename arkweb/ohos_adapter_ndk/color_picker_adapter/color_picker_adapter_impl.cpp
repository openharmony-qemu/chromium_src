/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "color_picker_adapter_impl.h"

#include <dlfcn.h>

#include "arkweb/ohos_nweb/src/nweb_hilog.h"

namespace OHOS::NWeb {
#if (defined(__aarch64__) || defined(__x86_64__))
const std::string COLOR_PICKER_SO_PATH =
    "/system/lib64/ndk/libcolorpicker_ndk.z.so";
#else
const std::string COLOR_PICKER_SO_PATH =
    "/system/lib/ndk/libcolorpicker_ndk.z.so";
#endif
const std::string FUNC_NAME = "HMS_GCP_StartColorPicker";
const std::string FUNC_WITH_VALUE_NAME =
    "HMS_GCP_StartColorPickerWithColorValue";

ColorPickerAdapterImpl::ColorPickerAdapterImpl() {
  library_handle_ = dlopen(COLOR_PICKER_SO_PATH.c_str(), RTLD_LAZY);
}

CallbackSharedWrapper<ColorPickerCallback>
    ColorPickerAdapterImpl::callback_wrapper_;

void ColorPickerNotify(void* user_data,
                       HMS_GCP_PickedColorInfo color_info,
                       const int32_t code) {
  if (user_data == nullptr) {
    WVLOG_E("ColorPickerNotify failed, user_data is NULL");
    return;
  }
  size_t callback_index = reinterpret_cast<size_t>(user_data);
  std::shared_ptr<ColorPickerCallback> color_picker_callback =
      ColorPickerAdapterImpl::callback_wrapper_.GetCallback(callback_index);

  if (color_picker_callback) {
    bool success = (code == 0);
    HMS_GCP_Color color = color_info.color;
    uint32_t value = (color.alpha << 24) | (color.red << 16) |
                     (color.green << 8) | color.blue;
    (*color_picker_callback)(success, value);
  }
}

void ColorPickerAdapterImpl::StartColorPicker(
    double x,
    double y,
    ColorPickerCallback color_picker_callback) {
  StartColorPickerInternal(x, y, color_picker_callback, FUNC_NAME);
}

void ColorPickerAdapterImpl::StartColorPickerWithColorValue(
    double x,
    double y,
    ColorPickerCallback color_picker_callback) {
  StartColorPickerInternal(x, y, color_picker_callback, FUNC_WITH_VALUE_NAME);
}

void ColorPickerAdapterImpl::StartColorPickerInternal(
    double x,
    double y,
    ColorPickerCallback color_picker_callback,
    std::string func_name) {
  HMS_GCP_StartColorPicker start_color_picker_func = nullptr;
  if (!library_handle_ ||
      !(start_color_picker_func = reinterpret_cast<HMS_GCP_StartColorPicker>(
            dlsym(library_handle_, func_name.c_str())))) {
    WVLOG_E("ColorPickerAdapterImpl::StartColorPicker failed");
    color_picker_callback(false, 0);  // Notify failure
    return;
  }
  if (callback_index_ > 0) {
    callback_wrapper_.Clear(callback_index_);
  }
  callback_index_ = callback_wrapper_.AddCallback(
      std::make_shared<ColorPickerCallback>(color_picker_callback));
  start_color_picker_func(x, y, ColorPickerNotify,
                          reinterpret_cast<void*>(callback_index_));
}

}  // namespace OHOS::NWeb
