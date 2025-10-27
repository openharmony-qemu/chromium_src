/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NWEB_VALUE_CONVERT_H
#define NWEB_VALUE_CONVERT_H

#include "cef/include/cef_base.h"
#include "cef/include/cef_values.h"
#include "nweb_hap_value.h"
#include "nweb_rom_value.h"
#include "nweb_value.h"
#include "nweb_web_message.h"

namespace OHOS::NWeb {
std::shared_ptr<NWebValue> AddNWebValueCef(CefRefPtr<CefValue> argument);

void AddNWebValueCefV2(CefRefPtr<CefValue> argument,
                       std::shared_ptr<NWebHapValue> value);

std::vector<std::shared_ptr<NWebValue>> ParseCefValueTONWebValue(
    CefRefPtr<CefListValue> args,
    int size);

CefValueType TranslateCefType(NWebValue::Type type);

CefRefPtr<CefValue> ParseNWebValueToValueHelper(
    std::shared_ptr<NWebValue> value);

CefRefPtr<CefListValue> ParseNWebValueToValue(std::shared_ptr<NWebValue> value,
                                              CefRefPtr<CefListValue> result);

std::vector<std::shared_ptr<NWebHapValue>> ParseCefValueToHapValue(
    CefRefPtr<CefListValue> args,
    int size);

CefRefPtr<CefListValue> ParseNWebValueToHapValue(
    std::shared_ptr<NWebHapValue> value,
    CefRefPtr<CefListValue> result);

CefRefPtr<CefValue> ParseHapValueToValueHelper(
    std::shared_ptr<NWebHapValue> value);

CefRefPtr<CefValue> ParseRomValueToValueHelper(
    std::shared_ptr<NWebRomValue> value);

std::shared_ptr<NWebMessage> ConvertNwebHap2NwebMessage(std::shared_ptr<NWebHapValue> hap);
}  // namespace OHOS::NWeb
#endif
