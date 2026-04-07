/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "extensions/browser/extension_icon_aggregator.h"

#include "base/logging.h"
#include "ohos_nweb/src/cef_delegate/nweb_extension_action_cef_delegate.h"

namespace extensions {

ExtensionIconAggregator::ExtensionIconAggregator() = default;

ExtensionIconAggregator::ExtensionIconAggregator(float icon_scale)
    : icon_scale_(icon_scale) {}

ExtensionIconAggregator::~ExtensionIconAggregator() = default;

void ExtensionIconAggregator::AddIcon(IconImage* icon_image,
                                      IconReadyCallback callback) {
  DCHECK(icon_image);
  DCHECK(callback);
  DCHECK(!started_);
  pending_icons_[icon_image] = std::move(callback);
}

void ExtensionIconAggregator::SetOnAllReadyCallback(
    base::OnceClosure callback) {
  on_all_ready_callback_ = std::move(callback);
}

void ExtensionIconAggregator::StartLoading() {
  LOG(INFO) << "ExtensionIconAggregator start loading";
  if (started_) {
    return;
  }
  started_ = true;

  if (pending_icons_.empty()) {
    TryTriggerAllReady();
    return;
  }

  std::vector<IconImage*> images_to_load;
  for (const auto& [image, callback] : pending_icons_) {
    images_to_load.push_back(image);
  }

  for (IconImage* image : images_to_load) {
    if (image->did_complete_initial_load()) {
      auto it = pending_icons_.find(image);
      if (it != pending_icons_.end()) {
        std::move(it->second)
            .Run(OHOS::NWeb::CreateNWebIconFromImageSkiaRepsV2(
                image->image().AsImageSkia().image_reps()));
        pending_icons_.erase(it);
      }
    } else {
      icon_observations_.AddObservation(image);
      image->image().AsImageSkia().GetRepresentation(icon_scale_);
    }
  }

  TryTriggerAllReady();
}

void ExtensionIconAggregator::OnExtensionIconImageChanged(IconImage* image) {
  LOG(INFO) << "ExtensionIconAggregator OnExtensionIconImageChanged";
  auto it = pending_icons_.find(image);
  if (it == pending_icons_.end()) {
    return;
  }

  if (!image->is_valid()) {
    LOG(ERROR) << "ExtensionIconAggregator: icon image is invalid";
    pending_icons_.erase(it);
    TryTriggerAllReady();
    return;
  }

  std::move(it->second)
      .Run(OHOS::NWeb::CreateNWebIconFromImageSkiaRepsV2(
          image->image().AsImageSkia().image_reps()));
  pending_icons_.erase(it);

  TryTriggerAllReady();
}

void ExtensionIconAggregator::TryTriggerAllReady() {
  if (!started_) {
    return;
  }

  if (!pending_icons_.empty()) {
    return;
  }

  if (on_all_ready_callback_) {
    LOG(INFO) << "ExtensionIconAggregator all ready";
    std::move(on_all_ready_callback_).Run();
  }
}

}  // namespace extensions
