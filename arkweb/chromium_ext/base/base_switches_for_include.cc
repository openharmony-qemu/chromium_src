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

#include "arkweb/build/features/features.h"

#if BUILDFLAG(IS_ARKWEB)
// virtual pixel ratio for ohos scrollbar
const char kPixelRatio[] = "virtual-pixel-ratio";
#endif

#if BUILDFLAG(IS_ARKWEB)
const char kBundleInstallationDir[] = "bundle-installation-dir";
const char kForTest[] = "for-test";
const char kBundleName[] = "bundle-name";
const char kOhosAppApiVersion[] = "user-api-version";
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
const char kEnableVideoAssistant[] = "enable-nweb-ex-video-assistant";
#endif // ARKWEB_VIDEO_ASSISTANT
