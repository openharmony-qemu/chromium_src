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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_DOM_DOCUMENT_UTILS_H_

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
#include "base/command_line.h"
#include "arkweb/chromium_ext/base/ohos/ltpo/include/touch_observer.h"
#include "content/public/common/content_switches.h"
#include "third_party/blink/renderer/core/render_mojom/render_mojom_client.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_fetcher.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

namespace blink {

class DocumentUtils : public GarbageCollected<DocumentUtils>{
 public:
  void Trace(Visitor*) const;
#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
  static constexpr uint64_t MAX_TOUCH_UP_INTERVAL = 500000000;
  static const int LOAD_URL_DELAY_TIME = 250;
#endif

#if BUILDFLAG(ARKWEB_PRP_PRELOAD)
  void DocumentInitializeUtils(const Member<ResourceFetcher>& fetcher_,
                                      const KURL& base_url_);
#endif

#if BUILDFLAG(ARKWEB_SLIDE_LTPO)
  static void StartBoostingUtils();
  void SetURLUtils();
#endif
};

}

#endif  // CC_MOJO_EMBEDDER_ASYNC_LAYER_TREE_FRAME_SINK_H_
