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

#include "arkweb/chromium_ext/content/browser/renderer_host/navigator_utils.h"

#include "content/browser/renderer_host/frame_tree.h"
#include "content/browser/renderer_host/navigation_request.h"
#include "content/browser/renderer_host/render_frame_host_impl.h"

namespace content {

NavigatorUtils::NavigatorUtils(Navigator* impl) : navigator_(impl) {}

void NavigatorUtils::StopPage()
{
  content::StoragePartition* storage_partition =
      navigator_->controller_.GetBrowserContext()->GetDefaultStoragePartition();
  if (storage_partition) {
    network::mojom::NetworkContext* network_context =
        storage_partition->GetNetworkContext();
    if (network_context != nullptr) {
      network_context->StopPage(reinterpret_cast<int64_t>(navigator_));
    }
  }
}

const net::NetworkAnonymizationKey NavigatorUtils::GetNetworkAnonymizationKey(
    FrameTreeNode* frame_tree_node,
    NavigationRequest* navigation_request)
{
  return frame_tree_node->current_frame_host()->ComputeIsolationInfoForNavigation(
    navigation_request->common_params().url,
    navigation_request->is_credentialless(),
    navigation_request->ComputeFencedFrameNonce()).network_anonymization_key();
}

}  // namespace content
