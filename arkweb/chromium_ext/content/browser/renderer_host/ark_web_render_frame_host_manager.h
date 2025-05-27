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

#ifndef ARKWEB_CHROMIUM_EXT_CONTENT_BROWSER_RENDERER_HOST_ARK_WEB_RENDER_FRAME_HOST_MANAGER_H_
#define ARKWEB_CHROMIUM_EXT_CONTENT_BROWSER_RENDERER_HOST_ARK_WEB_RENDER_FRAME_HOST_MANAGER_H_

#include <string>

#include "content/browser/site_instance_impl.h"
#include "content/common/content_export.h"
#include "content/public/browser/back_forward_cache.h"

namespace content {

CONTENT_EXPORT void ArkWebUnloadOldFrame(const BackForwardCache&,
                                         const std::string reason,
                                         bool& can_store);

CONTENT_EXPORT void ArkWebGetFrameHostForNavigation(
    const std::string& shared_render_process_token,
    SiteInstanceImpl* dest_site_instance);

}  // namespace content

#endif  // ARKWEB_CHROMIUM_EXT_CONTENT_BROWSER_RENDERER_HOST_ARK_WEB_RENDER_FRAME_HOST_MANAGER_H_
