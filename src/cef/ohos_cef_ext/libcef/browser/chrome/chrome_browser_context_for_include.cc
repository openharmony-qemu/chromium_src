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

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_CLOUD_CONTROL)
void ChromeBrowserContext::OnWebViewShow() {
  ScheduleUpdateCloudControl(this->AsBrowserContext());
}

void ChromeBrowserContext::OnContextInitialized() {
  ScheduleUpdateCloudControl(this->AsBrowserContext());
}
#endif

#if BUILDFLAG(ARKWEB_CLOUD_CONTROL) && !BUILDFLAG(ARKWEB_NWEB_EX)
void ChromeBrowserContext::ScheduleUpdateCloudControl(
    content::BrowserContext* context) {}
#endif
