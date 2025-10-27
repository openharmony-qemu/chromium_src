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

#ifndef NWEB_WEBVIEW_NATIVE_ARKWEB_UTILS_H
#define NWEB_WEBVIEW_NATIVE_ARKWEB_UTILS_H

#include "native_interface_arkweb.h"
#include "nweb.h"

void OH_NativeArkWeb_BindWebTagToWebInstance(const char* webTag, std::weak_ptr<OHOS::NWeb::NWeb> nwebPtr);
std::weak_ptr<OHOS::NWeb::NWeb> OH_NativeArkWeb_GetWebInstanceByWebTag(const char* webTag);

#endif // NWEB_WEBVIEW_NATIVE_ARKWEB_UTILS_H