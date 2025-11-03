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

#ifndef CONTENT_PUBLIC_BROWSER_RENDER_PROCESS_HOST_OHOS_H_
#define CONTENT_PUBLIC_BROWSER_RENDER_PROCESS_HOST_OHOS_H_

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
namespace nweb_ex {
struct BrowserReaderModeConfigData;
}
#endif  // ARKWEB_READER_MODE

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
namespace nweb_ex {
struct AlloyVideoLoadOptimizationData;
}
#endif  // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

namespace content {
class RenderProcessHostOhos {
 public:
#if BUILDFLAG(ARKWEB_READER_MODE)
  virtual void UpdateReaderModeConfig(
      const nweb_ex::BrowserReaderModeConfigData* data) = 0;
#endif

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
  virtual void UpdateVideoLoadOptimizationConfig(
    nweb_ex::AlloyVideoLoadOptimizationData& data) = 0;
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION
};
}  // namespace content
#endif  // CONTENT_PUBLIC_BROWSER_RENDER_PROCESS_HOST_OHOS_H_