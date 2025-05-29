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

#ifndef CONTENT_BROWSER_RENDER_HOST_NAIGATION_UTILS_H_
#define CONTENT_BROWSER_RENDER_HOST_NAIGATION_UTILS_H_

#include "content/browser/renderer_host/navigator.h"

namespace content {

class NavigatorUtils {
 public:
  NavigatorUtils(Navigator* impl);
  void StopPage();
  const net::NetworkAnonymizationKey GetNetworkAnonymizationKey(
      FrameTreeNode* frame_tree_node,
      NavigationRequest* navigation_request);

 private:
  Navigator* navigator_;
};
}  // namespace content
#endif  // CONTENT_BROWSER_RENDER_HOST_NAIGATION_UTILS_H_
