/* Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "arkweb/chromium_ext/content/browser/host_zoom_map_impl_utils.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "base/trace_event/trace_event.h"
namespace content {
#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
const int64_t ZOOM_FREQUENCY_LIMIT = 30;
#endif
  HostZoomMapImplUtils::HostZoomMapImplUtils(HostZoomMapImpl* impl) {
    this->zoom_map_impl_ = impl;
  }

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool HostZoomMapImplUtils::IsZoomTooFast(base::Time last_modified, double level) {
    base::TimeDelta elapsed = last_modified - last_modified_;
    int64_t elapsed_ms = elapsed.InMilliseconds();
    if (elapsed_ms < ZOOM_FREQUENCY_LIMIT) {
      LOG(INFO) << "Throw zoom event because frequency limit";
      return true;
    }
    last_modified_ = last_modified;
    TRACE_EVENT1("cc", "HostZoomMapImpl::SetZoomLevelForHostInternal",
                 "zoom_level", level);
    return false;
  }
#endif
}  // namespace content