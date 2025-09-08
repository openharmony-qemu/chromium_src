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

#ifndef CONTENT_RENDERER_RENDERER_BLINK_PLATFORM_IMPL_EXT_H_
#define CONTENT_RENDERER_RENDERER_BLINK_PLATFORM_IMPL_EXT_H_

#include "content/renderer/renderer_blink_platform_impl.h"
#include "third_party/blink/public/platform/scheduler/web_thread_scheduler.h"

namespace blink {
namespace scheduler {
class WebThreadScheduler;
}
}  // namespace blink

namespace content {
class RendererBlinkPlatformImplExt : public RendererBlinkPlatformImpl {
 public:
  explicit RendererBlinkPlatformImplExt(
      blink::scheduler::WebThreadScheduler* main_thread_scheduler);

  RendererBlinkPlatformImplExt(const RendererBlinkPlatformImpl&) = delete;
  RendererBlinkPlatformImplExt& operator=(const RendererBlinkPlatformImplExt&) =
      delete;

#if BUILDFLAG(ARKWEB_READER_MODE)
  const blink::mojom::ReaderModeConfig* GetReaderModeConfig() override;
#endif
};
}  // namespace content
#endif  // CONTENT_RENDERER_RENDERER_BLINK_PLATFORM_IMPL_EXT_H_