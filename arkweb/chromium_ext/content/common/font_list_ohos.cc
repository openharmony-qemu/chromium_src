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

#include "content/common/font_list.h"

#include <memory>

#include "base/logging.h"
#include "base/values.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace content {

base::Value::List GetFontList_SlowBlocking() {
  void* font_array = nullptr;
  OHOS::NWeb::ArkWeb_Drawing_SystemFontType type =
      OHOS::NWeb::ArkWeb_Drawing_SystemFontType::ALL;
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetOhosDrawingTextFontAdapter()
      .GetSystemFontFullNamesByType(type,
                                    reinterpret_cast<void**>(&font_array));
  int32_t size = 0;
  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetOhosDrawingTextTypographyAdapter()
      .GetDrawingArraySize(font_array, size);

  base::Value::List font_list;
  for (int32_t index = 0; index < size; ++index) {
    const void* full_name = nullptr;
    OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetOhosDrawingTextFontAdapter()
        .GetSystemFontFullNameByIndex(font_array, index, &full_name);
    OHOS::NWeb::ArkWeb_Drawing_FontDescriptor* description = nullptr;
    const OHOS::NWeb::ArkWeb_Drawing_String* drawing_string =
        static_cast<const OHOS::NWeb::ArkWeb_Drawing_String*>(full_name);
    OHOS::NWeb::ArkWeb_Drawing_String* none_const_drawing_string =
        const_cast<OHOS::NWeb::ArkWeb_Drawing_String*>(drawing_string);
    OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetOhosDrawingTextFontAdapter()
        .GetFontDescriptorByFullName(none_const_drawing_string, type,
                                     reinterpret_cast<void**>(&description));
    if (description == nullptr) {
      LOG(ERROR) << __FUNCTION__ << " font description null at: " << index;
      continue;
    }
    std::string font_id(description->postScriptName);
    std::string font_name(description->fullName);

    base::Value::List font_item;
    font_item.Append(font_id);
    font_item.Append(font_name);
    font_list.Append(std::move(font_item));

    OHOS::NWeb::OhosAdapterHelper::GetInstance()
        .GetOhosDrawingTextFontAdapter()
        .DestroyFontDescriptor(description);
    description = nullptr;
  }

  OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetOhosDrawingTextFontAdapter()
      .DestroySystemFontFullNames(font_array);
  font_array = nullptr;

  std::sort(font_list.begin(), font_list.end());
  return font_list;
}

}  // namespace content
