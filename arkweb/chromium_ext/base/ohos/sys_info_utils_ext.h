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

#define APP_API_LEVEL_20 20

#include "arkweb/build/features/features.h"

#include <stdint.h>

#include <string>

#include "base/base_export.h"

#include "build/buildflag.h"

namespace base {
namespace ohos {
BASE_EXPORT float GetPixelRatio();

BASE_EXPORT void SetPixelRatio(float ratio);

#if BUILDFLAG(ARKWEB_SAME_LAYER)
BASE_EXPORT float GetDevicePixelRatio();
BASE_EXPORT void SetDevicePixelRatio(float ratio);
#endif

#if BUILDFLAG(ARKWEB_TEST)
#ifdef __cplusplus
extern "C" {
#endif
#endif
BASE_EXPORT bool IsMobileDevice();

BASE_EXPORT bool IsTabletDevice();
#if BUILDFLAG(ARKWEB_TEST)
#ifdef __cplusplus
}
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

BASE_EXPORT bool IsPcDevice();

#ifdef __cplusplus
}
#endif

BASE_EXPORT bool IsEmulator();

BASE_EXPORT bool IsWearableDevice();

#if BUILDFLAG(ARKWEB_TEST)
#ifdef __cplusplus
extern "C" {
#endif
#endif
BASE_EXPORT int32_t MajorVersion();

BASE_EXPORT int32_t SeniorVersion();
#if BUILDFLAG(ARKWEB_TEST)
#ifdef __cplusplus
}
#endif
#endif

BASE_EXPORT std::string OsName();

#if BUILDFLAG(ARKWEB_TEST)
#ifdef __cplusplus
extern "C" {
#endif
#endif
BASE_EXPORT std::string OsVersion();

BASE_EXPORT std::string BaseOsName();

BASE_EXPORT std::string CompatibleDeviceType();
#if BUILDFLAG(ARKWEB_TEST)
#ifdef __cplusplus
}
#endif
#endif

BASE_EXPORT bool IsCompatibleMode();

BASE_EXPORT int32_t ApplicationApiVersion();

BASE_EXPORT void SetPageScale(bool status);

BASE_EXPORT bool IsPageScale();

BASE_EXPORT std::string ComponentName();

BASE_EXPORT bool IsPcMode();
}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_SYS_INFO_UTILS_H_
