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

#include "arkweb/chromium_ext/content/browser/renderer_host/ark_web_back_forward_cache_impl.h"

#if BUILDFLAG(ARKWEB_BFCACHE)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "content/browser/renderer_host/back_forward_cache_impl.h"
#include "content/public/common/content_switches.h"
#endif

bool GetArkWebBackForwardCacheFeatures(
    blink::scheduler::WebSchedulerTrackedFeatures& features) {
#if BUILDFLAG(ARKWEB_BFCACHE)
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableCacheNativeEmbed)) {
    auto feature = blink::scheduler::StringToFeature("EnableCacheNativeEmbed");
    if (feature.has_value()) {
      features.Put(feature.value());
    }
  }

  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableCacheMediaTakeOver)) {
    auto feature =
        blink::scheduler::StringToFeature("EnableCacheMediaTakeOver");
    if (feature.has_value()) {
      features.Put(feature.value());
    }
  }
  return true;
#else
  return false;
#endif  // BUILDFLAG(ARKWEB_BFCACHE)
}

namespace content {

namespace {
// The default time to live in seconds for documents in BackForwardCache.
// See also crbug.com/1305878.
static constexpr int kDefaultTimeToLiveInBackForwardCacheInSeconds = 600;

// Max BFCache size.
constexpr int kMaxBackForwardCacheSize = 50;
}  // namespace

#if BUILDFLAG(ARKWEB_BFCACHE)
//static
BFCacheFeatureConfigDataEx* BFCacheFeatureConfigDataEx::GetInstance() {
  static base::NoDestructor<BFCacheFeatureConfigDataEx> instance;
  return instance.get();
}

BFCacheFeatureConfigDataEx::BFCacheFeatureConfigDataEx() {}

BFCacheFeatureConfigDataEx::~BFCacheFeatureConfigDataEx() = default;

void BackForwardCacheImpl::SetCacheSize(int size) {
  if (size <= 0) {
    size = 0;
  } else if (size > kMaxBackForwardCacheSize) {
    size = kMaxBackForwardCacheSize;
  }

  this->size_ = size;
  LOG(INFO) << "BackForwardCacheImpl set backforward cache size: " << size;
  EnforceCacheSizeLimit();
}

base::TimeDelta BackForwardCacheImpl::ArkWebGetTimeToLiveInBackForwardCache() {
  if (base::CommandLine::ForCurrentProcess()->HasSwitch(
          switches::kEnableBFCache)) {
    return base::Seconds(this->time_to_live_);
  }

  return base::Seconds(kDefaultTimeToLiveInBackForwardCacheInSeconds);
}
#endif

#if BUILDFLAG(ARKWEB_BFCACHE)
size_t BackForwardCacheImpl::GetStoredEntriesNumber() {
  return entries_.size();
}
#endif

}  // namespace content
