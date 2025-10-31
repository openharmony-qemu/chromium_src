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

#ifndef COLOR_PICKER_ADAPTER_IMPL_H
#define COLOR_PICKER_ADAPTER_IMPL_H

#include "arkweb/ohos_adapter_ndk/interfaces/color_picker_adapter.h"
#include "arkweb/ohos_adapter_ndk/ndk_callback_wrapper/callback_shared_wrapper.h"

namespace OHOS::NWeb {

class ColorPickerAdapterImpl : public ColorPickerAdapter {
 public:
  ColorPickerAdapterImpl();
  ~ColorPickerAdapterImpl();

  void StartColorPicker(double x,
                        double y,
                        ColorPickerCallback color_picker_callback) override;
  void StartColorPickerWithColorValue(
      double x,
      double y,
      ColorPickerCallback color_picker_callback) override;

  static CallbackSharedWrapper<ColorPickerCallback> callback_wrapper_;

 private:
  void StartColorPickerInternal(double x,
                                double y,
                                ColorPickerCallback color_picker_callback,
                                std::string func_name);

  size_t callback_index_ = 0;
  void* library_handle_ = nullptr;
};

}  // namespace OHOS::NWeb

#endif  // COLOR_PICKER_ADAPTER_IMPL_H
