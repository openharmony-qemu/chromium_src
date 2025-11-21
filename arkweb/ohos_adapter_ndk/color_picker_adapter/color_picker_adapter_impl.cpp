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
const int32_t STYLUS_OK = 0;

typedef enum {
  /** an unknown color space */
  HMS_GCP_UNKNOWN = 0,
  /** the color space based on Adobe RGB (1998) */
  HMS_GCP_ADOBE_RGB_1998 = 1,
  /** the color space based on SMPTE RP 431-2-2007 and IEC 61966-2.1:1999 */
  HMS_GCP_DCI_P3 = 2,
  /** the color space based on SMPTE RP 431-2-2007 and IEC 61966-2.1:1999 */
  HMS_GCP_DISPLAY_P3 = 3,
  /** the standard red green blue (SRGB) color space based on IEC 61966-2.1:1999
   */
  HMS_GCP_SRGB = 4,
  /** the color space based on ITU-R BT.709, PRIMARIES_BT709 | TRANSFUNC_BT709 |
     RANGE_FULL */
  HMS_GCP_BT709 = 6,
  /** the color space based on ITU-R BT.601, PRIMARIES_BT601_P | TRANSFUNC_BT709
     | RANGE_FULL */
  HMS_GCP_BT601_EBU = 7,
  /** the color space based on ITU-R BT.601, PRIMARIES_BT601_N | TRANSFUNC_BT709
     | RANGE_FULL */
  HMS_GCP_BT601_SMPTE_C = 8,
  /** the color space based on ITU-R BT.2020, PRIMARIES_BT2020 | TRANSFUNC_HLG |
     RANGE_FULL */
  HMS_GCP_BT2020_HLG = 9,
  /** the color space based on ITU-R BT.2020, PRIMARIES_BT2020 | TRANSFUNC_PQ |
     RANGE_FULL */
  HMS_GCP_BT2020_PQ = 10,
  /** PRIMARIES_P3_D65 | TRANSFUNC_HLG | RANGE_FULL */
  HMS_GCP_P3_HLG = 11,
  /** PRIMARIES_P3_D65 | TRANSFUNC_PQ | RANGE_FULL */
  HMS_GCP_P3_PQ = 12,
  /** PRIMARIES_ADOBE_RGB | TRANSFUNC_ADOBE_RGB | RANGE_LIMIT */
  HMS_GCP_ADOBE_RGB_1998_LIMIT = 13,
  /** PRIMARIES_P3_D65 | TRANSFUNC_SRGB | RANGE_LIMIT */
  HMS_GCP_DISPLAY_P3_LIMIT = 14,
  /** PRIMARIES_SRGB | TRANSFUNC_SRGB | RANGE_LIMIT */
  HMS_GCP_SRGB_LIMIT = 15,
  /** PRIMARIES_BT709 | TRANSFUNC_BT709 | RANGE_LIMIT */
  HMS_GCP_BT709_LIMIT = 16,
  /** PRIMARIES_BT601_P | TRANSFUNC_BT709 | RANGE_LIMIT */
  HMS_GCP_BT601_EBU_LIMIT = 17,
  /** PRIMARIES_BT601_N | TRANSFUNC_BT709 | RANGE_LIMIT */
  HMS_GCP_BT601_SMPTE_C_LIMIT = 18,
  /** PRIMARIES_BT2020 | TRANSFUNC_HLG | RANGE_LIMIT */
  HMS_GCP_BT2020_HLG_LIMIT = 19,
  /** PRIMARIES_BT2020 | TRANSFUNC_PQ | RANGE_LIMIT */
  HMS_GCP_BT2020_PQ_LIMIT = 20,
  /** PRIMARIES_P3_D65 | TRANSFUNC_HLG | RANGE_LIMIT */
  HMS_GCP_P3_HLG_LIMIT = 21,
  /** PRIMARIES_P3_D65 | TRANSFUNC_PQ | RANGE_LIMIT */
  HMS_GCP_P3_PQ_LIMIT = 22,
  /** PRIMARIES_P3_D65 | TRANSFUNC_LINEAR */
  HMS_GCP_LINEAR_P3 = 23,
  /** PRIMARIES_SRGB | TRANSFUNC_LINEAR */
  HMS_GCP_LINEAR_SRGB = 24,
  /** PRIMARIES_BT2020 | TRANSFUNC_LINEAR */
  HMS_GCP_LINEAR_BT2020 = 25,
  /** a customized color space */
  CUSTOM = 5,
} HMS_GCP_ColorSpace;

typedef struct {
  /** The red channel. */
  int32_t red;
  /** The green channel. */
  int32_t green;
  /** The blue channel. */
  int32_t blue;
  /** The alpha channel. */
  int32_t alpha;
} HMS_GCP_Color;

typedef struct {
  /** The picked color value. */
  HMS_GCP_Color color;
  /** The colorspace of color present in. */
  HMS_GCP_ColorSpace colorSpace;
  /** The timestamp of color picked. */
  int64_t timestamp;
} HMS_GCP_PickedColorInfo;

using HMS_GCP_OnResult = void (*)(void* userData,
                                  HMS_GCP_PickedColorInfo colorInfo,
                                  const int32_t code);

using HMS_GCP_StartColorPicker = int32_t (*)(int32_t initialPosX,
                                             int32_t initialPosY,
                                             HMS_GCP_OnResult onResultCallback,
                                             void* userData);

ColorPickerAdapterImpl::ColorPickerAdapterImpl() {
  library_handle_ = dlopen(COLOR_PICKER_SO_PATH.c_str(), RTLD_LAZY);
}

ColorPickerAdapterImpl::~ColorPickerAdapterImpl() {
  if (library_handle_) {
    dlclose(library_handle_);
    library_handle_ = nullptr;
  }
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
    bool success = (code == STYLUS_OK);
    HMS_GCP_Color color = color_info.color;
    uint32_t value = (color.alpha << 24) | (color.red << 16) |
                     (color.green << 8) | color.blue;
    (*color_picker_callback)(success, value);
  }
  ColorPickerAdapterImpl::callback_wrapper_.Clear(callback_index);
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
  callback_index_ = callback_wrapper_.AddCallback(
      std::make_shared<ColorPickerCallback>(color_picker_callback));
#if defined(USE_LIBFUZZER) || BUILDFLAG(ARKWEB_TEST)
  HMS_GCP_PickedColorInfo color_info;
  ColorPickerNotify(reinterpret_cast<void*>(callback_index_), color_info, 0);
#else
  WVLOG_I("ColorPickerAdapterImpl::StartColorPicker start");
  int32_t ret = start_color_picker_func(
      x, y, ColorPickerNotify, reinterpret_cast<void*>(callback_index_));
  if (ret != STYLUS_OK) {
    WVLOG_E("ColorPickerAdapterImpl::StartColorPicker error");
    color_picker_callback(false, 0);  // Notify error
    callback_wrapper_.Clear(callback_index_);
  }
#endif
}

}  // namespace OHOS::NWeb
