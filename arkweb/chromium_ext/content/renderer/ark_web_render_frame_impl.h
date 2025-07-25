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

#ifndef ARKWEB_CHROMIUM_EXT_CONTENT_RENDERER_ARK_WEB_RENDER_FRAME_IMPL_H_
#define ARKWEB_CHROMIUM_EXT_CONTENT_RENDERER_ARK_WEB_RENDER_FRAME_IMPL_H_

#include <optional>
#include <chrono>

#include "arkweb/build/features/features.h"
#include "content/common/content_export.h"
#include "content/public/renderer/render_frame.h"
#include "third_party/blink/public/platform/web_string.h"

namespace content {

CONTENT_EXPORT std::optional<blink::WebString> ArkWebUserAgentOverride(
    content::RenderFrame* render_frame);

class RenderFrameImpl;

class RenderFrameImplUtils {
public:
    RenderFrameImpl* renderFrameImpl;
    RenderFrameImplUtils(RenderFrameImpl* impl);

#if BUILDFLAG(ARKWEB_DFX_TRACING)
static int64_t GetCurrentTimestampMS();
void ReportRenderInitBlock();
static constexpr int64_t kMicrosecondsPerMillisecond = 1000;
void ChangeCommitNavigationTime(int64_t time);
void ChangeCompleteInitialize(bool complete);
#endif

private:
#if BUILDFLAG(ARKWEB_DFX_TRACING)
  int64_t commit_navigation_time_ = 0;
  bool is_complete_initialize = false;
#endif
};

}  // namespace content

#endif  // ARKWEB_CHROMIUM_EXT_CONTENT_RENDERER_ARK_WEB_RENDER_FRAME_IMPL_H_
