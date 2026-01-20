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

#ifndef ARKWEB_CHROMIUM_EXT_CONTENT_BROWSER_RENDERER_HOST_ARK_WEB_BACK_FORWARD_CACHE_IMPL_H_
#define ARKWEB_CHROMIUM_EXT_CONTENT_BROWSER_RENDERER_HOST_ARK_WEB_BACK_FORWARD_CACHE_IMPL_H_

#include "third_party/blink/public/common/scheduler/web_scheduler_tracked_feature.h"
#if BUILDFLAG(ARKWEB_BFCACHE)
#include "base/no_destructor.h"
#include "content/common/content_export.h"
#endif

bool GetArkWebBackForwardCacheFeatures(
    blink::scheduler::WebSchedulerTrackedFeatures& features);

namespace content {

#if BUILDFLAG(ARKWEB_BFCACHE)
class CONTENT_EXPORT BFCacheFeatureConfigDataEx {
 public:
  static BFCacheFeatureConfigDataEx* GetInstance();
  void SetCacheControlNoStoreEnabled(bool value) {
    cache_control_no_store_enabled_ = value;
  }
  bool GetCacheControlNoStoreEnabled() {
    return cache_control_no_store_enabled_;
  }

 private:
  friend class base::NoDestructor<BFCacheFeatureConfigDataEx>;
  BFCacheFeatureConfigDataEx();
  ~BFCacheFeatureConfigDataEx();
  bool cache_control_no_store_enabled_ = false;
};
#endif // ARKWEB_BFCACHE

}
#endif  // ARKWEB_CHROMIUM_EXT_CONTENT_BROWSER_RENDERER_HOST_ARK_WEB_BACK_FORWARD_CACHE_IMPL_H_
