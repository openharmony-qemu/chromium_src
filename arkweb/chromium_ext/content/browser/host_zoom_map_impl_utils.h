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

#ifndef CONTENT_BROWSER_HOST_ZOOM_MAP_IMPL_UTILS_H_
#define CONTENT_BROWSER_HOST_ZOOM_MAP_IMPL_UTILS_H_

#include "content/browser/host_zoom_map_impl.h"

namespace content {
class CONTENT_EXPORT HostZoomMapImplUtils {
 public:
  raw_ptr<HostZoomMapImpl> zoom_map_impl_;
  HostZoomMapImplUtils(HostZoomMapImpl* impl);

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
  bool IsZoomTooFast(const std::string& host, base::Time last_modified, double level);
  typedef std::map<std::string, base::Time> HostLastModified;
  HostLastModified host_last_modified_;
#endif
};
}  // namespace content
#endif  // CONTENT_BROWSER_HOST_ZOOM_MAP_IMPL_UTILS_H_
