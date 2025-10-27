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

#ifndef CONTENT_PUBLIC_RENDERER_RENDER_THREAD_OHOS_H_
#define CONTENT_PUBLIC_RENDERER_RENDER_THREAD_OHOS_H_

#include "build/build_config.h"
#include "build/buildflag.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

namespace blink {
#if BUILDFLAG(ARKWEB_READER_MODE)
namespace mojom {
class ReaderModeConfig;
}
#endif
}  // namespace blink

namespace content {
class RenderThreadOhos {
 public:
#if BUILDFLAG(ARKWEB_READER_MODE)
  virtual const blink::mojom::ReaderModeConfig* GetReaderModeConfig() = 0;
#endif
};
}  // namespace content
#endif  // CONTENT_PUBLIC_RENDERER_RENDER_THREAD_OHOS_H_
