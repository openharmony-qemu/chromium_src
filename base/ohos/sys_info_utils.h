/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef BASE_OHOS_SYS_INFO_UTILS_H_
#define BASE_OHOS_SYS_INFO_UTILS_H_

#include <stdint.h>

#include <string>

#include "base/base_export.h"

namespace base {
namespace ohos {
#ifdef OHOS_SCROLLBAR
BASE_EXPORT float GetPixelRatio();

BASE_EXPORT void SetPixelRatio(float ratio);
#endif

BASE_EXPORT bool IsMobileDevice();

BASE_EXPORT bool IsTabletDevice();

BASE_EXPORT bool IsPcDevice();

BASE_EXPORT bool IsEmulator();

BASE_EXPORT int32_t MajorVersion();

BASE_EXPORT int32_t SeniorVersion();

BASE_EXPORT std::string OsName();


}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_SYS_INFO_UTILS_H_
