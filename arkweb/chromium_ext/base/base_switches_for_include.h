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

#ifndef ARKWEB_CHROMIUM_EXT_BASE_BASE_SWITCHES_FOR_INCLUDE_H_
#define ARKWEB_CHROMIUM_EXT_BASE_BASE_SWITCHES_FOR_INCLUDE_H_

#include "arkweb/build/features/features.h"

#if BUILDFLAG(IS_ARKWEB)
// virtual pixel ratio for ohos scrollbar
extern const char kPixelRatio[];
#endif

#if BUILDFLAG(IS_ARKWEB)
extern const char kBundleInstallationDir[];
extern const char kForTest[];
extern const char kBundleName[];
extern const char kOhosAppApiVersion[];
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
extern const char kEnableVideoAssistant[];
#endif  // ARKWEB_VIDEO_ASSISTANT

#endif  // ARKWEB_CHROMIUM_EXT_BASE_BASE_SWITCHES_FOR_INCLUDE_H_
